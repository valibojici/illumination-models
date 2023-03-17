#include "SceneMenu.h"

void SceneMenu::onRenderImGui()
{
	if (ImGui::Button("Test scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new TriangleScene(m_currentScene);
		delete temp;
	}
	if (ImGui::Button("Simple 3D scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new Simple3dScene(m_currentScene);
		delete temp;
	}
	if (ImGui::Button("Mesh test scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new MeshTestScene(m_currentScene);
		delete temp;
	}
	if (ImGui::Button("Texture test scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new TextureTestScene(m_currentScene);
		delete temp;
	}
	if (ImGui::Button("Model test scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new ModelTestScene(m_currentScene);
		delete temp;
	}

}

void SceneMenu::onRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
