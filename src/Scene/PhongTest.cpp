#include "PhongTest.h"

PhongTest::PhongTest(Scene*& scene)
    : Scene(scene)
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

    // setting uniforms
    // TODO: get screen size from config class?
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);

    for (auto& shader : m_shaders) {
        shader.bind();
        shader.setMat4("u_projMatrix", m_projMatrix);
        shader.setBool("u_gammaCorrect", m_gammaCorrection);
        shader.setInt("u_numLights", 3);
        for (auto& light : m_lights) {
            light->setUniforms(shader);
        }
    }


    // set up materials
    for (int model = 0; model < 3; ++model) {
        // walls
        std::vector<std::unique_ptr<Material>> wallMaterials;
        for (unsigned int i = 0; i < m_transforms.size(); ++i) {
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

    // bind current shader
    m_shaders[m_modelIndex].bind();
}

PhongTest::~PhongTest()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
}

void PhongTest::onRender()
{
    static double time = glfwGetTime();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // update camera position and uniforms
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
    m_shaders[m_modelIndex].setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shaders[m_modelIndex].setVec3("u_viewPos", m_camera.getPosition());
    // lights
    for (auto& light : m_lights) {
        light->setUniforms(m_shaders[m_modelIndex]);
        light->draw(m_shaders[m_modelIndex]);
    }
    
    // draw box
    for (unsigned int i = 0; i < m_transforms.size(); ++i) {
        m_wallMaterials[m_modelIndex][i]->setUniforms(m_shaders[m_modelIndex]);
        m_shaders[m_modelIndex].setMat4("u_modelMatrix", m_transforms[i]);
        m_wall->draw(m_shaders[m_modelIndex]);
    }

    // draw mesh
    m_materials[m_modelIndex]->setUniforms(m_shaders[m_modelIndex]);
    m_shaders[m_modelIndex].setMat4("u_modelMatrix", m_modelMatrix);
    m_mesh->draw(m_shaders[m_modelIndex]);
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

    if (ImGui::Checkbox("Enable gamma correction", &m_gammaCorrection)) {
        m_shaders[m_modelIndex].setBool("u_gammaCorrect", m_gammaCorrection);
    }

    // enable/disable wireframes, for debug
    static bool showWireFrames = false;
    if (ImGui::Checkbox("Show wireframe", &showWireFrames)) {
        glPolygonMode(GL_FRONT_AND_BACK, showWireFrames ? GL_LINE : GL_FILL);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
