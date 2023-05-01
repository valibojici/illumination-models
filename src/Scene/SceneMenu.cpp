#include "SceneMenu.h"

void SceneMenu::onRenderImGui()
{
	if (ImGui::Button("Model test scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new ModelTestScene(m_currentScene);
		delete temp;
	}
	if (ImGui::Button("Floor scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new Floor(m_currentScene);
		delete temp;
	}
	if (ImGui::Button("Box room scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new Box(m_currentScene);
		delete temp;
	}
}

void SceneMenu::onRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
