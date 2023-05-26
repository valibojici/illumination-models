#include "Scene.h"
#include "SceneMenu.h"

bool Scene::renderImGuiBackButton()
{
    // back button (make it red)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Back", ImVec2(50.0f, 20.0f))) {
        setScene(std::make_unique<SceneMenu>(m_currentScene, m_width, m_height));
        ImGui::PopStyleColor();
        return true;
    }
    ImGui::PopStyleColor();
    // add some spacing
    ImGui::NewLine();
    return false;
}

void Scene::helpPoput(const char* text)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
