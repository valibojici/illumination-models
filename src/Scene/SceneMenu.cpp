#include "SceneMenu.h"

void SceneMenu::onRenderImGui()
{
	if (ImGui::Button("Model test scene")) {
		setScene(std::make_unique<ModelTestScene>(m_currentScene, m_width, m_height));
	}
	if (ImGui::Button("Texture test scene")) {
		setScene(std::make_unique<TextureScene>(m_currentScene, m_width, m_height));
	}
	if (ImGui::Button("Box room scene")) {
		setScene(std::make_unique<Box>(m_currentScene, m_width, m_height));
	}
}

void SceneMenu::onRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
