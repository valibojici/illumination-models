#include "SceneMenu.h"

void SceneMenu::onRenderImGui()
{
	if (ImGui::Button("Test scene")) {
		Scene* temp = m_currentScene;
		m_currentScene = new TriangleScene(m_currentScene);
		delete temp;
	}
}

void SceneMenu::onRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
