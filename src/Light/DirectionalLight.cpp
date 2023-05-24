#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(int index, const glm::vec3& direction, const glm::vec3& color) : 
	Light(index, color)
{
	m_type = Type::DIRECTIONAL;
	m_position = direction;
	// set name
	std::stringstream ss;
	ss << "Directional light #" << index;
	m_name = ss.str();

	// set light projection parameters
	m_parameters.directional(-4, 4, -4, 4, 0.1f, 12.0f, 2.5f, { 0.0f, 0.0f, 1.0f });
	calculateLightSpaceMatrix();
}

void DirectionalLight::imGuiRender()
{
	Light::imGuiRender();

	if (ImGui::DragFloat3("Direction", &m_position.x, 0.01f)) {
		// set UP vector to positive Y if light is on Z axis
		m_parameters.UP = (m_position.x == 0.0f && m_position.y == 0.0f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
		calculateLightSpaceMatrix();
		m_shadowNeedsRender = true;
	}

	// UI for light view projection parameters
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.25f, 0.25f, 1.0f)); // Set header color
	if (ImGui::CollapsingHeader("Light projection matrix parameters")) {
		if (ImGui::DragFloat("Min x", &m_parameters.minx, 0.001f, -50.0f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
		if (ImGui::DragFloat("Max x", &m_parameters.maxx, 0.001f, -50.0f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
		if (ImGui::DragFloat("Min y", &m_parameters.miny, 0.001f, -50.0f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
		if (ImGui::DragFloat("Max y", &m_parameters.maxy, 0.001f, -50.0f, 50.0f)) {
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
		if (ImGui::DragFloat("Scale", &m_parameters.directionalLightScale, 0.01f, 0.1f, 50.0f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
	}
	ImGui::PopStyleColor();
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
	shader.setBool(formatAttribute("shadow"), m_shadow);
	shader.setMat4(formatAttribute("lightSpaceMatrix"), m_lightSpaceMatrix[0]);
	shader.setInt(formatAttribute("shadowMap"), m_shadowTextureSlot);
	shader.setFloat(formatAttribute("farPlane"), m_parameters.far_plane);
	shader.setInt(formatAttribute("type"), 0);
}

void DirectionalLight::calculateLightSpaceMatrix()
{
	// use orthographic projection for directional light
	m_lightSpaceMatrix[0] = glm::ortho(
		m_parameters.minx,
		m_parameters.maxx,
		m_parameters.miny,
		m_parameters.maxy,
		m_parameters.near_plane,
		m_parameters.far_plane) *
		glm::lookAt(
			m_parameters.directionalLightScale * glm::normalize(m_position), // "place" the light on a sphere with radius 'directionalLightScale'
			glm::vec3(0.0f), // looking at origin
			m_parameters.UP); // up vector
}