#include "Spotlight.h"

Spotlight::Spotlight(int index, const glm::vec3& position, const glm::vec3& target) : Light(index)
{
	std::stringstream ss;
	ss << "Spot light #" << index;
	m_name = ss.str();
	m_position = position;
	m_target = target;
}

void Spotlight::imGuiRender(Shader& shader)
{
	ImGui::PushID(this);
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
	
	if (ImGui::CollapsingHeader(m_name.c_str())) {

		Light::imGuiRender(shader);

		if (ImGui::DragFloat3("Position", &m_position.x, 0.1f, -5.0f, 5.0f)) {
			shader.setVec4(formatAttribute("position"), m_modelMatrix * glm::vec4(m_position, 1.0f));
		}

		if (ImGui::DragFloat3("Target", &m_target.x, 0.1f, -5.0f, 5.0f)) {
			shader.setVec3(formatAttribute("target"), m_target);
		}

		/*
		* the cut off and outer cut off are stored in degrees 
		* but the shader expects cos values
		*/
		if (ImGui::DragFloat("Cut off", &m_cutOff, 0.1f, 0.0f, std::min(m_outerCutOff, 90.0f), "%.1f deg")) {
			shader.setFloat(formatAttribute("cutOff"), glm::cos(glm::radians(m_cutOff)));
		}
		if (ImGui::DragFloat("Outer cut off", &m_outerCutOff, 0.1f, m_cutOff, 90.0f, "%.1f deg")) {
			shader.setFloat(formatAttribute("outerCutOff"), glm::cos(glm::radians(m_outerCutOff)));
		}

		// slider is logaritmic for greater control
		if (ImGui::SliderFloat3("Attenuation", &m_attenuation[0], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) {
			shader.setVec3(formatAttribute("attenuation"), m_attenuation);
		}
	}
	ImGui::NewLine();
	ImGui::PopStyleColor();
	ImGui::PopID();
}

void Spotlight::setUniforms(Shader& shader)
{
	shader.setVec4(formatAttribute("position"), m_modelMatrix * glm::vec4(m_position, 1.0f));
	shader.setFloat(formatAttribute("intensity"), m_intensity);
	shader.setVec3(formatAttribute("color"), m_color);
	shader.setBool(formatAttribute("enabled"), m_enabled);
	shader.setVec3(formatAttribute("attenuation"), m_attenuation);
	shader.setVec3(formatAttribute("target"), m_target);
	shader.setFloat(formatAttribute("cutOff"), glm::cos(glm::radians(m_cutOff))); // shader uses cos values
	shader.setFloat(formatAttribute("outerCutOff"), glm::cos(glm::radians(m_outerCutOff))); // shader uses cos values
}
