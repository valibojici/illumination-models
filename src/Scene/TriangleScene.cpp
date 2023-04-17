#include "TriangleScene.h"

TriangleScene::TriangleScene(Scene*& scene)
    : Scene(scene)
{
    m_shader.load("shader.vert", "shader.frag");
    m_shader.bind();

    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f)),
        Vertex(glm::vec3(0.0f, 0.5f, 0.0f)),
    };
    m_vbo.bufferData(vertices.data(), sizeof(Vertex) * vertices.size());
    m_vao.create();
    m_vao.addLayout(VAO::DataType::FLOAT, 3); // position
    m_vao.addLayout(VAO::DataType::FLOAT, 2); // texture
    m_vao.addLayout(VAO::DataType::FLOAT, 3); // normal
    m_vao.addLayout(VAO::DataType::FLOAT, 3); // tangent
    m_vao.linkVBO(m_vbo);
}

TriangleScene::~TriangleScene()
{

}

void TriangleScene::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_shader.setVec3("u_color", m_color);
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TriangleScene::onRenderImGui()
{
    if (ImGui::Button("Back")) {
        Scene* temp = m_currentScene;
        m_currentScene = new SceneMenu(m_currentScene);
        delete temp;
    }

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::ColorEdit3("clear color", (float*)&m_color.x); // Edit 3 floats representing a color
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
