#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(int index, const glm::vec3& direction) : Light(index)
{
	std::stringstream ss;
	ss << "Directional light #" << index;
	m_name = ss.str();
	m_position = direction;
}

void DirectionalLight::imGuiRender(Shader& shader)
{
	ImGui::PushID(this);
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
	if (ImGui::CollapsingHeader(m_name.c_str())) {
		Light::imGuiRender(shader);

		ImGui::DragFloat3("Direction", &m_position.x, 0.1f, -5.0f, 5.0f);
	}
	ImGui::NewLine();
	ImGui::PopStyleColor();
	ImGui::PopID();
}

void DirectionalLight::setUniforms(Shader& shader)
{
	shader.setVec4(formatAttribute("position"), m_modelMatrix * glm::vec4(m_position, 0.0f));
	shader.setFloat(formatAttribute("intensity"), m_intensity);
	shader.setVec3(formatAttribute("color"), m_color);
	shader.setBool(formatAttribute("enabled"), m_enabled);
	shader.setVec3(formatAttribute("attenuation"), glm::vec3(0.0f));
	shader.setVec3(formatAttribute("target"), glm::vec3(0.0f));
	shader.setFloat(formatAttribute("cutOff"), glm::cos(glm::radians(0.0f)));
	shader.setFloat(formatAttribute("outerCutOff"), glm::cos(glm::radians(0.0f)));
}
