#include "Spotlight.h"

Spotlight::Spotlight(int index, const glm::vec3& position, const glm::vec3& target, const glm::vec3& color, float cutOff, float outerCutoff) 
	: Light(index, color)
{
	m_type = Type::SPOT;
	std::stringstream ss;
	ss << "Spot light #" << index;
	m_name = ss.str();
	m_position = position;
	m_target = target;
	m_cutOff = cutOff;
	m_outerCutOff = outerCutoff;
	
	// set light projection parameters
	m_parameters.spotlight(1.0f, 1.0f, 12.0f, { 0.0f, 0.0f, 1.0f });
	calculateLightSpaceMatrix();
}

void Spotlight::imGuiRender()
{
	Light::imGuiRender();

	if (ImGui::DragFloat3("Position", &m_position.x, 0.01f)) {
		// set UP vector to positive Z if light is on Y axis
		m_parameters.UP = (m_position.z == 0.0f && m_position.x == 0.0f) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
		calculateLightSpaceMatrix();
		m_shadowNeedsRender = true;
	}

	if (ImGui::DragFloat3("Target", &m_target.x, 0.01f)) {
		calculateLightSpaceMatrix();
		m_shadowNeedsRender = true;
	}
	/*
	* the cut off and outer cut off are stored in degrees (shader expects cos values)
	*/
	if (ImGui::DragFloat("Cut off", &m_cutOff, 0.1f, 0.1f, 90.0f, "%.1f deg")) {
		m_outerCutOff = std::max(m_outerCutOff, m_cutOff);
		calculateLightSpaceMatrix();
		m_shadowNeedsRender = true;
	}
	if (ImGui::DragFloat("Outer cut off", &m_outerCutOff, 0.1f, 0.1f, 90.0f, "%.1f deg")) {
		m_cutOff = std::min(m_cutOff, m_outerCutOff);
		calculateLightSpaceMatrix();
		m_shadowNeedsRender = true;
	}
	// slider is logaritmic for greater control
	ImGui::SliderFloat3("Attenuation", &m_attenuation[0], 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

	// UI for light view projection parameters
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.25f, 0.25f, 1.0f)); // Set header color
	if (ImGui::CollapsingHeader("Light projection matrix parameters")) {
		if (ImGui::DragFloat("Aspect", &m_parameters.aspect, 0.001f, 0.001f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
		if (ImGui::DragFloat("Near plane", &m_parameters.near_plane, 0.001f, 0.001f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
		if (ImGui::DragFloat("Far plane", &m_parameters.far_plane, 0.001f, 0.001f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
	}
	ImGui::PopStyleColor();
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
	shader.setBool(formatAttribute("shadow"), m_shadow);
	shader.setMat4(formatAttribute("lightSpaceMatrix"), m_lightSpaceMatrix[0]);
	shader.setInt(formatAttribute("shadowMap"), m_shadowTextureSlot);
	shader.setFloat(formatAttribute("farPlane"), m_parameters.far_plane);
	shader.setInt(formatAttribute("type"), 1);
}

void Spotlight::calculateLightSpaceMatrix()
{
	// use a perspective matrix
	m_lightSpaceMatrix[0] = glm::perspective(
		glm::radians(2.0f * m_outerCutOff),
		m_parameters.aspect,
		m_parameters.near_plane,
		m_parameters.far_plane) *
		glm::lookAt(
			m_position,
			m_target,
			m_parameters.UP);
}
