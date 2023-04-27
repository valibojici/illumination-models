#include "PhongTest.h"

PhongTest::PhongTest(Scene*& scene)
    : Scene(scene), m_hdrFBO(1280, 720), m_shadowFBO(2000, 2000)
{
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    EventManager::getInstance().addHandler(&m_camera);
    // enable depth testing
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    m_lights.push_back(std::move(std::make_unique<PointLight>(0, glm::vec3(1.5f, 2.0f, 1.0f))));
    m_lights.push_back(std::move(std::make_unique<DirectionalLight>(1, glm::vec3(0.0f, 2.0f, 0.0f))));
    m_lights.push_back(std::move(std::make_unique<Spotlight>(2, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f))));

    // load shaders
    m_shaders.resize(3);
    m_shaders[0].load("base_shader.vert", "phong.frag");
    m_shaders[1].load("base_shader.vert", "blinn.frag");
    m_shaders[2].load("base_shader.vert", "cook-torrance.frag");
    m_postprocessShader.load("postprocess.vert", "postprocess.frag");
    m_shadowShader.load("shadowmap.vert", "shadowmap.frag");

    m_postProcessUI.addShaders({ &m_shaders[0], &m_shaders[1], &m_shaders[2], &m_postprocessShader });
    m_postProcessUI.setUniforms();

    // setting uniforms
    // TODO: get screen size from config class?
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
    for (int model = 0; model < 3; ++model) {
        // walls
        std::vector<std::unique_ptr<Material>> wallMaterials;
        for (size_t i = 0; i < m_transforms.size(); ++i) {
            std::unique_ptr<Material> wallMaterial;
            switch (model)
            {
            case 0:
                wallMaterial = std::make_unique<PhongMaterial>();
                break;
            case 1:
                wallMaterial = std::make_unique<BlinnMaterial>();
                break;
            case 2:
                wallMaterial = std::make_unique<CookTorranceMaterial>();
                break;
            }

            switch (i)
            {
            case 0: // left wall - red
                wallMaterial->setColor({ 0.92f, 0.25f, 0.20f });
                wallMaterial->setAmbient({ 0.92f, 0.25f, 0.20f });
                break;
            case 2: // right wall - blue
                wallMaterial->setColor({ 0.2, 0.51, 0.92 });
                wallMaterial->setAmbient({ 0.2, 0.51, 0.92 });
                break;
            default:
                wallMaterial->setColor({ 0.8f, 0.8f, 0.8f });
                wallMaterial->setAmbient({ 0.8f, 0.8f, 0.8f });
            }
            // disable specular highlights for walls
            wallMaterial->disableHighlights();
            wallMaterials.push_back(std::move(wallMaterial));
        }
        m_wallMaterials.push_back(std::move(wallMaterials));

        // main mesh
        std::unique_ptr<Material> material;
        switch (model)
        {
        case 0:
            material = std::make_unique<PhongMaterial>();
            break;
        case 1:
            material = std::make_unique<BlinnMaterial>();
            break;
        case 2:
            material = std::make_unique<CookTorranceMaterial>();
            break;
        }
        m_materials.push_back(std::move(material));
    }

    // set up HDR framebuffer
    m_hdrFBO.addColorAttachament(true, GL_RGBA16F);
    m_hdrFBO.addDepthAttachment(false);
    m_hdrFBO.create();

    // set up shadow related variables

    // no shadow for point lights
    m_lights[1]->setShadow(true);
    m_lights[2]->setShadow(true);
    
    // for every light that casts shadow create a depth buffer
    for (size_t i = 0; i < m_lights.size(); ++i) {
        if (!m_lights[i]->getShadow())continue;
        unsigned int id = m_shadowFBO.addDepthAttachment();
        // set the depth maps starting from slot 8 
        glActiveTexture(GL_TEXTURE8 + i);
        glBindTexture(GL_TEXTURE_2D, id);
        m_lights[i]->setShadowTextureSlot(8 + i);
    }
    
    m_shadowFBO.create(); // create the shadow framebuffer
}

PhongTest::~PhongTest()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
}

void PhongTest::setLightSpaceMatrices()
{
    // the matrices depend on the scene

    // directional light
    m_lights[1]->setLightSpaceMatrix(
        glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 6.0f) *
        glm::lookAt(
            2.5f * glm::normalize(m_lights[1]->getPosition()), // "place" the light on a sphere with radius 2.5
            glm::vec3(0.0f), // looking at origin
            glm::vec3(0.0f, 0.0f, 1.0f)) // up vector is towards the screen
    );

    // spotlight
    m_lights[2]->setLightSpaceMatrix(
        glm::perspective(
            glm::radians(2.0f * dynamic_cast<Spotlight*>(m_lights[2].get())->getOuterCutoff()), // multiply by 2 so the scene is in view
            1.0f,
            0.5f,
            12.0f) *
        glm::lookAt(
            m_lights[2]->getPosition(),
            dynamic_cast<Spotlight*>(m_lights[2].get())->getTarget(),
            glm::vec3(0.0f, 0.0f, 1.0f))
    );
}


void PhongTest::onRender()
{
    static double time = glfwGetTime();
    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    m_shaders[m_modelIndex].bind();
    m_shaders[m_modelIndex].setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shaders[m_modelIndex].setVec3("u_viewPos", m_camera.getPosition());

    /******************
    * SHADOW PASS
    ******************/

    // setup viewport and framebuffer
    glViewport(0, 0, 2000, 2000);
    m_shadowFBO.bind();
    // enable depth testing and face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    m_shadowShader.bind();
    
    setLightSpaceMatrices();

    for (size_t i = 0, shadowTextureIndex = 0; i < m_lights.size(); ++i) {
        if (!m_lights[i]->getShadow()) continue;
        // activate the depth attachment for this light
        m_shadowFBO.activateDepthAttachment(shadowTextureIndex);
        shadowTextureIndex++;

        m_shadowShader.setMat4("u_lightSpaceMatrix", m_lights[i]->getLightSpaceMatrix());
        glClear(GL_DEPTH_BUFFER_BIT);

        // draw mesh
        glCullFace(GL_BACK);
        m_shadowShader.setMat4("u_modelMatrix", m_modelMatrix);
        m_mesh->draw(m_shadowShader);

        glCullFace(GL_BACK);
        // draw box
        for (size_t j = 0; j < m_transforms.size(); ++j) {
            m_shadowShader.setMat4("u_modelMatrix", m_transforms[j]);
            m_wall->draw(m_shadowShader);
        }
    }

    /******************
    * LIGHTING PASS
    ******************/
    glViewport(0, 0, 1280, 720);
    m_hdrFBO.bind();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

    // draw box
    for (size_t i = 0; i < m_transforms.size(); ++i) {
        m_wallMaterials[m_modelIndex][i]->setUniforms(m_shaders[m_modelIndex]);
        m_shaders[m_modelIndex].setMat4("u_modelMatrix", m_transforms[i]);
        m_wall->draw(m_shaders[m_modelIndex]);
    }

    // draw mesh
    m_shaders[m_modelIndex].bind();
    m_materials[m_modelIndex]->setUniforms(m_shaders[m_modelIndex]);
    m_shaders[m_modelIndex].setMat4("u_modelMatrix", m_modelMatrix);
    m_mesh->draw(m_shaders[m_modelIndex]);

    if (m_wireframeEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_hdrFBO.unbind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    m_screenQuadRenderer.render(m_hdrFBO.getColorAttachment(0), m_postprocessShader);
}

void PhongTest::onRenderImGui()
{
    // back button
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene);
        delete temp;
        return;
    }

    // render UI for every light
    for (auto &light : m_lights) {
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
