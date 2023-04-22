#include "FloorScene.h"

FloorScene::FloorScene(Scene*& scene) : Scene(scene), m_fbo(1280, 720)
{
    m_camera = Camera({ 0.0f, 1.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(-5.0f, 1.0f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(1, glm::vec3(0.0f, 1.0f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<PointLight>(2, glm::vec3(5.0f, 1.0f, 0.0f))));

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
        shader.setInt("u_numLights", 3);
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }
    

    // set up materials
    m_materials.push_back(std::move(std::make_unique<PhongMaterial>()));
    m_materials.push_back(std::move(std::make_unique<BlinnMaterial>()));
    m_materials.push_back(std::move(std::make_unique<CookTorranceMaterial>()));

    //set up textures
    std::vector<std::shared_ptr<Texture>> textures = {
        TextureManager::get().getTexture("textures/floor.png", Texture::Type::DIFFUSE),
        TextureManager::get().getTexture("textures/floor_normal.png", Texture::Type::NORMAL),
        TextureManager::get().getTexture("textures/floor_specular.png", Texture::Type::SPECULAR),
        TextureManager::get().getTexture("textures/floor_roughness.png", Texture::Type::ROUGHNESS),
    };
    m_mesh->setTextures(textures);

    m_modelMatrix = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // bind current shader
    m_shaders[m_modelIndex].bind();

    m_fbo.addColorAttachament();
    m_fbo.addDepthAttachment(false);
    m_fbo.create();
}

FloorScene::~FloorScene()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
}

void FloorScene::onRender()
{
    static double time = glfwGetTime();
    
    m_fbo.bind();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    m_shaders[m_modelIndex].bind();
    m_shaders[m_modelIndex].setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shaders[m_modelIndex].setVec3("u_viewPos", m_camera.getPosition());

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    }

    // lights
    for (auto& light : m_lights) {
        light->setUniforms(m_shaders[m_modelIndex]);
        light->draw(m_shaders[m_modelIndex]);
    }

    // draw mesh
    m_materials[m_modelIndex]->setUniforms(m_shaders[m_modelIndex]);
    m_shaders[m_modelIndex].setMat4("u_modelMatrix", m_modelMatrix);
    m_mesh->draw(m_shaders[m_modelIndex]);

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_fbo.unbind();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_fbo.getColorAttachment(0), m_postprocessShader);
}

void FloorScene::onRenderImGui()
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
        light->imGuiRender(m_shaders[m_modelIndex]);
    }

    if (ImGui::Combo("Lighting model", &m_modelIndex, "Phong\0Blinn-Phong\0Cook-Torrance\0\0")) {
        m_shaders[m_modelIndex].bind();
    }
    ImGui::NewLine();

    // render UI for material
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Material settings")) {
        m_materials[m_modelIndex]->imGuiRender(m_shaders[m_modelIndex]);
    }
    ImGui::PopID();

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
