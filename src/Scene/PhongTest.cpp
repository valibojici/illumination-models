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
    m_shader.load("shaders/phong.vert", "shaders/phong.frag");
    m_shader.bind();

    // setting uniforms
    // TODO: get screen size from config class?
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);
    m_shader.setMat4("u_projMatrix", m_projMatrix);
    m_shader.setBool("u_gammaCorrect", m_gammaCorrection);
    m_shader.setInt("u_numLights", 3);
    for (auto& light : m_lights) {
        light->setUniforms(m_shader);
    }

    // set up materials for walls
    for (unsigned int i = 0; i < m_transforms.size(); ++i) {
        m_wallMaterials.push_back(std::move(std::make_unique<PhongMaterial>()));
        switch (i)
        {
        case 0: // left wall - red
            m_wallMaterials[i]->setDiffuseColor({ 0.92f, 0.25f, 0.20f });
            m_wallMaterials[i]->setAmbientColor({ 0.92f, 0.25f, 0.20f });
            break;
        case 2: // right wall - blue
            m_wallMaterials[i]->setDiffuseColor({ 0.2, 0.51, 0.92 });
            m_wallMaterials[i]->setAmbientColor({ 0.2, 0.51, 0.92 });
            break;
        default:
            m_wallMaterials[i]->setDiffuseColor({ 0.8f, 0.8f, 0.8f });
            m_wallMaterials[i]->setAmbientColor({ 0.8f, 0.8f, 0.8f });
            m_wallMaterials[i]->setAmbientCoefficient(0.001f);
        }
        // disable specular highlights for walls
        m_wallMaterials[i]->setSpecularCoefficient(0);
    }
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
    m_shader.setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shader.setVec3("u_viewPos", m_camera.getPosition());
    
    // draw box
    for (unsigned int i = 0; i < m_transforms.size(); ++i) {
        m_wallMaterials[i]->setUniforms(m_shader);
        m_shader.setMat4("u_modelMatrix", m_transforms[i]);
        m_wall->draw(m_shader);
    }

    // draw mesh
    m_material->setUniforms(m_shader);
    m_shader.setMat4("u_modelMatrix", m_modelMatrix);
    m_mesh->draw(m_shader);
    
    // draw lights
    for (auto &light : m_lights) {
        light->draw(m_shader);
    }
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
        light->imGuiRender(m_shader);
    }

    // render UI for material
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Material settings")) {
        m_material->imGuiRender(m_shader);
    }
    ImGui::PopID();

    // button to reset camera position and orientation
    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }

    if (ImGui::Checkbox("Enable gamma correction", &m_gammaCorrection)) {
        m_shader.setBool("u_gammaCorrect", m_gammaCorrection);
    }

    // enable/disable wireframes, for debug
    static bool showWireFrames = false;
    if (ImGui::Checkbox("Show wireframe", &showWireFrames)) {
        glPolygonMode(GL_FRONT_AND_BACK, showWireFrames ? GL_LINE : GL_FILL);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
