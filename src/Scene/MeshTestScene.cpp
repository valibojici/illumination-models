#include "MeshTestScene.h"

MeshTestScene::MeshTestScene(Scene*& scene)
    : Scene(scene)
{
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    m_shader.load("simpleLighting.vert", "simpleLighting.frag");
    m_shader.bind();
 
    m_shader.setVec3("u_color", m_color);
    // TODO: get screen size from config class?
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);
    m_shader.setMat4("u_projMatrix", m_projMatrix);

    m_shader.setVec3("u_lightPos", m_lightPos);
    EventManager::getInstance().addHandler(&m_camera);
    glEnable(GL_CULL_FACE);
}

MeshTestScene::~MeshTestScene()
{
    glDisable(GL_CULL_FACE);
    EventManager::getInstance().removeHandler(&m_camera);
    delete m_mesh;
}

void MeshTestScene::onRender()
{
    static double time = glfwGetTime();
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();

    m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_shader.setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shader.setVec3("u_color", m_color); //
    m_shader.setVec3("u_specColor", m_specColor); //
    m_shader.setFloat("u_diffuseFactor", m_diffuseFactor); // 
    m_shader.setFloat("u_specFactor", m_specFactor); //
    m_shader.setFloat("u_shininess", m_shininess); // 
    m_shader.setVec3("u_viewPos", m_camera.getPosition());
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_mesh->draw(m_shader);
}

void MeshTestScene::onRenderImGui()
{
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene);
        delete temp;
    }
    ImGui::ColorEdit3("Color", &m_color.x);
    ImGui::ColorEdit3("Specular color", &m_specColor.x);
    ImGui::DragFloat("Diffuse factor", &m_diffuseFactor, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Specular factor", &m_specFactor, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Shininess", &m_shininess, 0.1f, 1.0f, 1024.0f);
    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }
    if (ImGui::DragFloat("Light position", &m_lightRotationAngle, 1.0f, 0.0f, 360.0f)) {
        auto rotationMatrix = glm::rotate(glm::radians(m_lightRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        m_shader.setVec3("u_lightPos", glm::vec4(m_lightPos, 1.0f) * rotationMatrix);
    }

    static bool showWireFrames = false;
    if (ImGui::Checkbox("Show wireframe", &showWireFrames)) {
        glPolygonMode(GL_FRONT_AND_BACK, showWireFrames ? GL_LINE : GL_FILL);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
