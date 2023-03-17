#include "ModelTestScene.h"

ModelTestScene::ModelTestScene(Scene*& scene)
    : Scene(scene)
{
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    m_shader.load("shaders/texture.vert", "shaders/texture.frag");
    m_shader.bind();

    // TODO: get screen size from config class?
    m_projMatrix = glm::infinitePerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f);
    m_shader.setMat4("u_projMatrix", m_projMatrix);

    m_shader.setVec3("u_lightPos", m_lightPos);
    EventManager::getInstance().addHandler(&m_camera);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    /*m_mesh->setTextures({
        TextureManager::get().getTexture("textures/brickwall.png"),
        TextureManager::get().getTexture("textures/brickwall.png",Texture::Type::SPECULAR),
        TextureManager::get().getTexture("textures/brickwall_normal.png",Texture::Type::NORMAL),
    });*/

    m_model.load("models/backpack/backpack.obj");
}

ModelTestScene::~ModelTestScene()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    EventManager::getInstance().removeHandler(&m_camera);
    //delete m_mesh;
    delete m_lightMesh;
}

void ModelTestScene::onRender()
{
    static double time = glfwGetTime();
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    m_shader.setMat4("u_viewMatrix", m_camera.getMatrix());
    m_shader.setVec3("u_viewPos", m_camera.getPosition());

    // draw model
    m_shader.setVec3("u_emission", glm::vec3(0.0f));
    m_shader.setMat4("u_modelMatrix", m_modelMatrix);
    m_model.draw(m_shader);

    // draw light
    m_shader.setVec3("u_emission", glm::vec3(1.0f));
    m_shader.setMat4("u_modelMatrix", glm::translate(glm::vec3(m_lightModelMatrix * glm::vec4(m_lightPos, 1.0f))));
    m_lightMesh->draw(m_shader);
}

void ModelTestScene::onRenderImGui()
{
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene);
        delete temp;
    }
    if (ImGui::DragFloat("Diffuse factor", &m_diffuseFactor, 0.001f, 0.0f, 1.0f)) {
        m_shader.setFloat("u_diffuseFactor", m_diffuseFactor);
    }

    if (ImGui::DragFloat("Specular factor", &m_specFactor, 0.001f, 0.0f, 1.0f)) {
        m_shader.setFloat("u_specFactor", m_specFactor);
    }

    if (ImGui::DragFloat("Shininess", &m_shininess, 0.1f, 1.0f, 1024.0f)) {
        m_shader.setFloat("u_shininess", m_shininess);
    }

    if (ImGui::Button("Reset camera")) {
        m_camera.setPosition({ 0.0f, 0.0f, 5.0f });
        m_camera.setTarget({ 0.0f, 0.0f, 0.0f });
    }

    if (ImGui::DragFloat("Light position", &m_lightRotationAngle, 0.75f, -360.0f, 360.0f)) {
        m_lightModelMatrix = glm::rotate(glm::radians(m_lightRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        m_shader.setVec3("u_lightPos", m_lightModelMatrix * glm::vec4(m_lightPos, 1.0f));
    }

    static float planeRotation = 0;
    if (ImGui::DragFloat("Plane rotation", &planeRotation, 0.75f, 0.0f, 180.0f)) {
        m_modelMatrix = glm::rotate(glm::radians(-planeRotation), glm::vec3(1.0f, 0.0f, 0.0f));
        m_shader.setMat3("u_transposeInverseModel", glm::transpose(glm::inverse(glm::mat3(m_modelMatrix))));
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
