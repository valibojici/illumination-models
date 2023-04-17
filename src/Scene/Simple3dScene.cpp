#include "Simple3dScene.h"

Simple3dScene::Simple3dScene(Scene*& scene)
    : Scene(scene)
{
    m_camera = Camera({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
    m_shader.load("shader.vert", "shader.frag");
    m_shader.bind();

    std::vector<Vertex> vertices = {
        Vertex({-1.0f, -1.0f, 1.0f}),
        Vertex({1.0f, -1.0f, 1.0f}),
        Vertex({1.0f, 1.0f, 1.0f}),
        Vertex({-1.0f, 1.0f, 1.0f}),

        Vertex({-1.0f, -1.0f, -1.0f}),
        Vertex({1.0f, -1.0f, -1.0f}),
        Vertex({1.0f, 1.0f, -1.0f}),
        Vertex({-1.0f, 1.0f, -1.0f}),
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0, //  front
        1, 5, 6, 6, 2, 1, // right
        5, 4, 7, 7, 6, 5, // back
        4, 0, 3, 3, 7, 4, // left
        3, 2, 6, 6, 7, 3, // top
        4, 5, 1, 1, 0, 4, // bottom
    };
    m_vao.create();
    m_vao.bind();
    m_vbo.bufferData(vertices.data(), sizeof(Vertex) * vertices.size());
    m_ebo.bufferData(indices.data(), sizeof(unsigned int) * indices.size());

    m_vao.addLayout(VAO::DataType::FLOAT, 3); // position
    m_vao.addLayout(VAO::DataType::FLOAT, 2); // texture
    m_vao.addLayout(VAO::DataType::FLOAT, 3); // normal
    m_vao.addLayout(VAO::DataType::FLOAT, 3); // tangent
    m_vao.linkVBO(m_vbo);

    m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // TODO: get screen size from config class?
    m_projMatrix = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 10.0f);
    m_shader.setMat4("u_viewMatrix", m_viewMatrix);
    m_shader.setMat4("u_projMatrix", m_projMatrix);

    EventManager::getInstance().addHandler(&m_camera);

    glEnable(GL_CULL_FACE);
}

Simple3dScene::~Simple3dScene()
{
    glDisable(GL_CULL_FACE);
    EventManager::getInstance().removeHandler(&m_camera);
}

void Simple3dScene::onRender()
{
    static double time = glfwGetTime();
    m_camera.update(glfwGetTime() - time);
    time = glfwGetTime();

    m_shader.setMat4("u_modelMatrix", glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationAngle), m_rotationAxis));
    m_shader.setMat4("u_viewMatrix", m_camera.getMatrix());
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_shader.setVec3("u_color", m_color);
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Simple3dScene::onRenderImGui()
{
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene);
        delete temp;
    }

    {
        ImGui::DragFloat("Rotation", &m_rotationAngle, 1.0f, 0.0f, 360.0f);
        ImGui::DragFloat3("Rotation axis", &m_rotationAxis.x, 0.01f, -1.0f, 1.0f);
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::ColorEdit3("clear color", (float*)&m_color.x); // Edit 3 floats representing a color
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
