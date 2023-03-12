#include "TextureTestScene.h"

TextureTestScene::TextureTestScene(Scene*& scene)
    : Scene(scene)
{
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    m_shader.load("shaders/texture.vert", "shaders/texture.frag");
    m_shader.bind();
 
    m_shader.setVec3("u_color", m_color);
    // TODO: get screen size from config class?
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);
    m_shader.setMat4("u_projMatrix", m_projMatrix);

    m_shader.setVec3("u_lightPos", m_lightPos);
    EventManager::getInstance().addHandler(&m_camera);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    m_textures.push_back(new Texture("textures/brickwall.png"));
    m_textures.push_back(new Texture("textures/brickwall.png", Texture::Type::SPECULAR));
    m_textures.push_back(new Texture("textures/brickwall_normal.png", Texture::Type::NORMAL));
    m_mesh->setTextures(m_textures);
}

TextureTestScene::~TextureTestScene()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
    delete m_mesh;
    delete m_lightMesh;
    for (auto t : m_textures) {
        delete t;
    }
}

void TextureTestScene::onRender()
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
    m_shader.setMat3("u_transposeInverseModel", glm::transpose(glm::inverse(glm::mat3(m_modelMatrix))));
    m_shader.setVec3("u_lightPos", m_lightModelMatrix * glm::vec4(m_lightPos, 1.0f));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_shader.setVec3("u_emission", glm::vec3(0.0f));
    m_shader.setMat4("u_modelMatrix", m_modelMatrix);
    m_mesh->draw(m_shader);

    m_shader.setVec3("u_emission", glm::vec3(1.0f));
    m_shader.setMat4("u_modelMatrix", glm::translate(glm::vec3(m_lightModelMatrix * glm::vec4(m_lightPos, 1.0f))));
    m_lightMesh->draw(m_shader);
}

void TextureTestScene::onRenderImGui()
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
    if (ImGui::DragFloat("Light position", &m_lightRotationAngle, 0.75f, -360.0f, 360.0f)) {
        m_lightModelMatrix = glm::rotate(glm::radians(m_lightRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    static float planeRotation = 0;
    if (ImGui::DragFloat("Plane rotation", &planeRotation, 0.75f, 0.0f, 180.0f)) {
        m_modelMatrix = glm::rotate(glm::radians(-planeRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    static bool showWireFrames = false;
    if (ImGui::Checkbox("Show wireframe", &showWireFrames)) {
        glPolygonMode(GL_FRONT_AND_BACK, showWireFrames ? GL_LINE : GL_FILL);
    }

    static bool normalMapping = false;
    if (ImGui::Checkbox("Enable normal mapping", &normalMapping)) {
        m_shader.setBool("u_normalMapping", normalMapping);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
