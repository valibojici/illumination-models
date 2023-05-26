#include "SceneMenu.h"

void SceneMenu::onRenderImGui()
{
	ImGui::Text("Select a scene from below:");
	if (ImGui::Button("Model test scene", ImVec2(ImGui::GetWindowSize().x * 0.5f, 30.0f))) {
		setScene(std::make_unique<ModelTestScene>(m_currentScene, m_width, m_height));
	}
	ImGui::Spacing();
	if (ImGui::Button("Texture test scene", ImVec2(ImGui::GetWindowSize().x * 0.5f, 30.0f))) {
		setScene(std::make_unique<TextureScene>(m_currentScene, m_width, m_height));
	}
	ImGui::Spacing();
	if (ImGui::Button("Box room scene", ImVec2(ImGui::GetWindowSize().x * 0.5f, 30.0f))) {
		setScene(std::make_unique<Box>(m_currentScene, m_width, m_height));
	}
}

void SceneMenu::onRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
