#include "Floor.h"

Floor::Floor(Scene*& scene,unsigned int width, unsigned int height) : 
    Scene(scene, width, height)
{
    updateWidthHeight(width, height);
    m_camera = Camera({ 0.0f, 1.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(-5.0f, 1.5f, -0.5f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(1, glm::vec3(0.0f, 1.5f, -0.5f))));

    // load shaders
    m_shaders.resize(3);
    m_shaders[0].load("base_shader.vert", "phong.frag");
    m_shaders[1].load("base_shader.vert", "blinn.frag");
    m_shaders[2].load("base_shader.vert", "cook-torrance.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    
    m_postProcessUI.addShaders({ &m_shaders[0], &m_shaders[1], &m_shaders[2], &m_postprocessShader });
    m_postProcessUI.setUniforms();

    for (auto& shader : m_shaders) {
        shader.bind();
        shader.setInt("u_numLights", m_lights.size());
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }

    // load meshes
    m_meshes.push_back(std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 4.0f)));
    m_meshes.push_back(std::unique_ptr<Mesh>(Mesh::getSphere(1.4f, 6)));

    // load textures
    m_textures.push_back({}); // no textures

    m_textures.push_back({
       TextureManager::get().getTexture("textures/wood_floor/diffuse.png", Texture::Type::DIFFUSE),
       TextureManager::get().getTexture("textures/wood_floor/normal.png", Texture::Type::NORMAL),
       TextureManager::get().getTexture("textures/wood_floor/roughness.png", Texture::Type::ROUGHNESS),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/chain_floor/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/chain_floor/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/chain_floor/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/chain_floor/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/chain_floor/opacity.png", Texture::Type::OPACITY),
        });
   
    m_textures.push_back({
        TextureManager::get().getTexture("textures/art_deco/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/art_deco/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/art_deco/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/art_deco/roughness.png", Texture::Type::ROUGHNESS),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/metal/diffuse.png", Texture::Type::DIFFUSE, false),
        TextureManager::get().getTexture("textures/metal/normal.png", Texture::Type::NORMAL, false),
        TextureManager::get().getTexture("textures/metal/metallic.png", Texture::Type::METALLIC, false),
        TextureManager::get().getTexture("textures/metal/roughness.png", Texture::Type::ROUGHNESS, false),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/wood_platform/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/wood_platform/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/wood_platform/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/wood_platform/metallic.png", Texture::Type::METALLIC),
        TextureManager::get().getTexture("textures/wood_platform/opacity.png", Texture::Type::OPACITY),
        });

    m_textures.push_back({
        TextureManager::get().getTexture("textures/grass/diffuse.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/grass/normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/grass/roughness.png", Texture::Type::ROUGHNESS),
        TextureManager::get().getTexture("textures/grass/metallic.png", Texture::Type::METALLIC),
        });
    
    MaterialMesh floor;
    floor.modelMatrix = glm::translate(glm::vec3(-5.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floor.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    floor.name = "Floor 1";
    floor.textureScaleX = 2.0f;
    floor.textureScaleY = 2.0f;
    floor.textureIndex = 1;
    m_materialMeshes.push_back(std::move(floor));

    floor = MaterialMesh();
    floor.modelMatrix = glm::rotate(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floor.materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    floor.materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));
    floor.name = "Floor 2";
    floor.textureScaleX = 2.0f;
    floor.textureScaleY = 2.0f;
    floor.textureIndex = 2;
    m_materialMeshes.push_back(std::move(floor));

    
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
    
    m_hdrFBO->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec3 clearColor = m_postProcessUI.getGammaCorrection() ? glm::pow(glm::vec3(0.1f), glm::vec3(2.2f)) : glm::vec3(0.1f);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
    glViewport(0, 0, m_width, m_height);

    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    for (auto& shader : m_shaders) {
        shader.setMat4("u_viewMatrix", m_camera.getMatrix());
        shader.setVec3("u_viewPos", m_camera.getPosition());
        shader.setMat4("u_projMatrix", m_projMatrix);
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
    for (auto& mesh : m_materialMeshes) {
        Shader& shader = m_shaders[mesh.modelIndex];
        mesh.materials[mesh.modelIndex]->setUniforms(shader);
        shader.setMat4("u_modelMatrix", mesh.modelMatrix);
        if (mesh.meshIndex == 1) {
            shader.setFloat("u_textureScaleX", glm::pi<float>() * mesh.textureScaleX);
            shader.setFloat("u_textureScaleY", mesh.textureScaleY);
        }
        else {
            shader.setFloat("u_textureScaleX", mesh.textureScaleX);
            shader.setFloat("u_textureScaleY", mesh.textureScaleY);
        }
        m_meshes[mesh.meshIndex]->setTextures(m_textures[mesh.textureIndex]);
        m_meshes[mesh.meshIndex]->draw(shader);
    }

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_hdrFBO->unbind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_hdrFBO->getColorAttachment(0), m_postprocessShader);
}

void Floor::onRenderImGui()
{
    // back button
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene, m_width, m_height);
        delete temp;
        return;
    }

    // render UI for every light
    for (size_t i = 0; i < m_lights.size(); ++i) {
        ImGui::PushID(m_lights[i].get());
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
        if (ImGui::CollapsingHeader(m_lights[i]->m_name.c_str())) {
            // render combox for light type
            int type = (int)m_lights[i]->getType();
            if (ImGui::Combo("Type", &type, "Point\0Directional\0Spotlight\0\0")) {
                switch (type)
                {
                case 0:
                    m_lights[i] = std::move(std::make_unique<PointLight>(i, m_lights[i]->getPosition()));
                    break;
                case 1:
                    m_lights[i] = std::move(std::make_unique<DirectionalLight>(i, m_lights[i]->getPosition()));
                    break;
                case 2:
                    m_lights[i] = std::move(std::make_unique<Spotlight>(i, m_lights[i]->getPosition(), glm::vec3(0.0f)));
                    break;
                }
            }
            // render the rest of UI
            m_lights[i]->imGuiRender();
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
        ImGui::NewLine();
    }
 
    ImGui::NewLine();

    // render UI for materials
    for (auto& mesh : m_materialMeshes) {
        ImGui::PushID(mesh.materials[mesh.modelIndex].get());
        if (ImGui::CollapsingHeader(mesh.name.c_str())) {
            ImGui::Combo("Texture", &mesh.textureIndex, "No texture\0Wood\0Chain\0Art Deco\0Rusted metal\0Wood platform\0Grass\0\0");
            ImGui::Combo("Mesh", &mesh.meshIndex, "Plane\0Sphere\0\0");
            ImGui::NewLine();
            ImGui::Combo("Lighting model", &mesh.modelIndex, "Phong\0Blinn-Phong\0Cook-Torrance\0\0");
            mesh.materials[mesh.modelIndex]->imGuiRender();
        }
        ImGui::NewLine();
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

void Floor::updateWidthHeight(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    m_hdrFBO = std::make_unique<Framebuffer>(width, height);
    m_hdrFBO->addColorAttachament(GL_TEXTURE_2D, GL_RGB16F);
    m_hdrFBO->addDepthAttachment(GL_TEXTURE_2D);
    m_hdrFBO->create();
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1.0f * m_width / m_height, 0.1f);
}
