#include "Floor.h"

Floor::Floor(Scene*& scene) : Scene(scene), m_hdrFBO(1280, 720)
{
    m_camera = Camera({ 0.0f, 1.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(-5.0f, 1.0f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(1, glm::vec3(0.0f, 1.0f, 0.0f))));

    m_lights.push_back(std::move(std::make_unique<PointLight>(2, glm::vec3(-5.0f, 1.0f, 10.0f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(3, glm::vec3(0.0f, 1.0f, 10.0f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(4, glm::vec3(5.0f, 1.0f, 10.0f))));

    // load shaders
    m_shaders.resize(3);
    m_shaders[0].load("base_shader.vert", "phong.frag");
    m_shaders[1].load("base_shader.vert", "blinn.frag");
    m_shaders[2].load("base_shader.vert", "cook-torrance.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    
    m_postProcessUI.addShaders({ &m_shaders[0], &m_shaders[1], &m_shaders[2], &m_postprocessShader });
    m_postProcessUI.setUniforms();

    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);

    for (auto& shader : m_shaders) {
        shader.bind();
        shader.setMat4("u_projMatrix", m_projMatrix);
        shader.setInt("u_numLights", m_lights.size());
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }
    
    MaterialMesh floor;
    floor.modelMatrix = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floor.mesh = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 8.0f));
    floor.mesh->setTextures({ 
        TextureManager::get().getTexture("textures/chain_floor/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/chain_floor/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/chain_floor/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/chain_floor/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/chain_floor/opacity.png", Texture::Type::OPACITY),
    });
    floor.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    floor.name = "Metal floor";
    floor.textureScaleX = 2.0f;
    floor.textureScaleY = 2.0f * 8.0f / 4.0f;
    m_meshes.push_back(std::move(floor));

    floor = MaterialMesh();
    floor.modelMatrix = glm::translate(glm::vec3(-5.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floor.mesh = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 8.0f));
    floor.mesh->setTextures({
        TextureManager::get().getTexture("textures/wood_floor/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/wood_floor/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/wood_floor/roughness.png", Texture::Type::ROUGHNESS),
    });
    floor.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    floor.name = "Wood floor";
    floor.textureScaleX = 2.0f;
    floor.textureScaleY = 2.0f * 8.0f / 4.0f;
    m_meshes.push_back(std::move(floor));

    for (int i = -1; i <= 1; ++i) {
        floor = MaterialMesh();
        floor.modelMatrix = glm::translate(glm::vec3(-5.0f * i, 0.0f, 10.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        floor.mesh = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 8.0f));
        floor.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
        floor.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
        floor.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
        for (auto& material : floor.materials) {
            material->setColor({ 0.5f, 0.5f, 0.5f });
            material->setAmbient({ 0.5f, 0.5f, 0.5f });
        }
        floor.name = std::string("Floor ") + std::to_string(i + 1);
        floor.textureScaleX = 2.0f;
        floor.textureScaleY = 2.0f * 8.0f / 4.0f;
        m_meshes.push_back(std::move(floor));
    }

    m_hdrFBO.addColorAttachament(GL_TEXTURE_2D, GL_RGB16F);
    m_hdrFBO.addDepthAttachment(GL_TEXTURE_2D);
    m_hdrFBO.create();
}

Floor::~Floor()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    EventManager::getInstance().removeHandler(&m_camera);
}

void Floor::onRender()
{
    static double time = glfwGetTime();
    
    m_hdrFBO.bind();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    for (auto& shader : m_shaders) {
        shader.setMat4("u_viewMatrix", m_camera.getMatrix());
        shader.setVec3("u_viewPos", m_camera.getPosition());
    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    }

    // lights
    for (auto& light : m_lights) {
        light->draw(m_shaders[0]);
        for (auto& shader : m_shaders) {
            light->setUniforms(shader);
        }
    }

    // draw mesh
    for (auto& mesh : m_meshes) {
        Shader& shader = m_shaders[mesh.modelIndex];
        mesh.materials[mesh.modelIndex]->setUniforms(shader);
        shader.setMat4("u_modelMatrix", mesh.modelMatrix);
        shader.setFloat("u_textureScaleX", mesh.textureScaleX);
        shader.setFloat("u_textureScaleY", mesh.textureScaleY);
        mesh.mesh->draw(shader);

    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_hdrFBO.unbind();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_hdrFBO.getColorAttachment(0), m_postprocessShader);
}

void Floor::onRenderImGui()
{
    // back button
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene);
        delete temp;
        return;
    }

    // render UI for every light
    for (auto& light : m_lights) {
        light->imGuiRender();
    }
 
    ImGui::NewLine();

    // render UI for materials
    for (auto& mesh : m_meshes) {
        ImGui::PushID(mesh.materials[mesh.modelIndex].get());
        if (ImGui::CollapsingHeader(mesh.name.c_str())) {
            ImGui::Combo("Lighting model", &mesh.modelIndex, "Phong\0Blinn-Phong\0Cook-Torrance\0\0");
            mesh.materials[mesh.modelIndex]->imGuiRender();
        }
        ImGui::PopID();
    }

    // button to reset camera position and orientation
    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }

    m_postProcessUI.onRenderImGui();

    // enable/disable wireframes, for debug
    ImGui::Checkbox("Show wireframe", &m_wireframeEnabled);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
