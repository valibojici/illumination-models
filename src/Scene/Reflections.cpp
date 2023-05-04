#include "Reflections.h"

Reflections::Reflections(Scene*& scene)
    : Scene(scene), m_hdrFBO(1280, 720)
{
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(1.5f, 2.0f, 1.0f))));
    m_lights.push_back(std::move(std::make_unique<DirectionalLight>(1, glm::vec3(0.0f, 2.0f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<Spotlight>(2, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f))));

    // load shaders
    m_shaders.resize(3);
    m_shaders[0].load("base_shader.vert", "phong.frag");
    m_shaders[1].load("base_shader.vert", "blinn.frag");
    m_shaders[2].load("base_shader.vert", "cook-torrance.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    m_skyboxShader.load("skybox.vert", "skybox.frag");

    m_postProcessUI.addShaders({ &m_shaders[0], &m_shaders[1], &m_shaders[2], &m_postprocessShader });
    m_postProcessUI.setUniforms();
    // setting uniforms
    // TODO: get screen size from config class?
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);

    for (auto& shader : m_shaders) {
        shader.bind();
        shader.setMat4("u_projMatrix", m_projMatrix);
        shader.setInt("u_numLights", m_lights.size());
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }

    // setup materials
    m_materials.push_back(std::make_unique<PhongMaterial>());
    m_materials.push_back(std::make_unique<BlinnMaterial>());
    m_materials.push_back(std::make_unique<CookTorranceMaterial>());

    // setup mesh
    m_mesh = std::unique_ptr<Mesh>(Mesh::getSphere(1.0f, 5));
    
    // set up HDR framebuffer
    m_hdrFBO.addColorAttachament(GL_TEXTURE_2D, GL_RGBA16F);
    m_hdrFBO.addDepthAttachment(GL_RENDERBUFFER);
    m_hdrFBO.create();

    // load skybox
    m_skyboxTexture = TextureManager::get().getTexture("textures/skybox", Texture::Type::CUBEMAP, false);

    // bind skybox to slot 1
    m_skyboxTexture->bind(1);
    // set skybox texture sampler in shaders
    for (auto& shader : m_shaders) {
        shader.setBool("u_useReflections", true);
        shader.setInt("u_reflectionMap", 1);
    }
}

Reflections::~Reflections()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
}

void Reflections::onRender()
{
    static double time = glfwGetTime();
    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    m_shaders[m_modelIndex].bind();
    m_shaders[m_modelIndex].setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shaders[m_modelIndex].setVec3("u_viewPos", m_camera.getPosition());

    m_hdrFBO.bind();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shaders[m_modelIndex].bind();


    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // lights
    for (size_t i = 0; i < m_lights.size(); ++i) {
        m_lights[i]->setUniforms(m_shaders[m_modelIndex]);
        m_lights[i]->draw(m_shaders[m_modelIndex]);
    }
    
    // draw mesh
    m_shaders[m_modelIndex].setMat4("u_modelMatrix", m_modelMatrix);
    m_materials[m_modelIndex]->setUniforms(m_shaders[m_modelIndex]);
    m_mesh->draw(m_shaders[m_modelIndex]);

    // ignore camera translation
    glm::mat4 invViewProj = glm::inverse(m_projMatrix * glm::mat4(glm::mat3(m_camera.getMatrix()))); 
    m_screenQuadRenderer.renderCubemap(m_skyboxTexture->getId(), m_skyboxShader, invViewProj);
    

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_hdrFBO.unbind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_hdrFBO.getColorAttachment(0), m_postprocessShader);
}

void Reflections::onRenderImGui()
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

    if (ImGui::Combo("Lighting model", &m_modelIndex, "Phong\0Blinn-Phong\0Cook-Torrance\0\0")) {
        m_shaders[m_modelIndex].bind();
    }
    ImGui::NewLine();

    // render UI for material
    m_materials[m_modelIndex]->imGuiRender();

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
