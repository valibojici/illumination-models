#include "PointLight.h"

PointLight::PointLight(int index, const glm::vec3& position) : Light(index)
{
	std::stringstream ss;
	ss << "Point light #" << index;
	m_name = ss.str();
	m_position = position;
}

void PointLight::imGuiRender(Shader& shader)
{
	ImGui::PushID(this);
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
	if (ImGui::CollapsingHeader(m_name.c_str())) {

		Light::imGuiRender(shader);

		ImGui::DragFloat3("Position", &m_position.x, 0.1f, -5.0f, 5.0f);
		ImGui::SliderFloat3("Attenuation", &m_attenuation[0], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	}
	ImGui::NewLine();
	ImGui::PopStyleColor();
	ImGui::PopID();
}

void PointLight::setUniforms(Shader& shader)
{
	shader.setVec4(formatAttribute("position"), m_modelMatrix * glm::vec4(m_position, 1.0f));
	shader.setFloat(formatAttribute("intensity"), m_intensity);
	shader.setVec3(formatAttribute("color"), m_color);
	shader.setBool(formatAttribute("enabled"), m_enabled);
	shader.setVec3(formatAttribute("attenuation"), m_attenuation);
	shader.setVec3(formatAttribute("target"), glm::vec3(0.0f));
	shader.setFloat(formatAttribute("cutOff"), glm::cos(glm::radians(0.0f)));
	shader.setFloat(formatAttribute("outerCutOff"), glm::cos(glm::radians(0.0f)));
}
