#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(int index, const glm::vec3& direction, const glm::vec3& color) : 
	Light(index, color)
{
	m_type = Type::DIRECTIONAL;
	std::stringstream ss;
	ss << "Directional light #" << index;
	m_name = ss.str();
	m_position = direction;
}

void DirectionalLight::imGuiRender()
{
	ImGui::PushID(this);
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set header color
	if (ImGui::CollapsingHeader(m_name.c_str())) {
		Light::imGuiRender();

		if (ImGui::DragFloat3("Direction", &m_position.x, 0.01f)) {
			calculateLightSpaceMatrix();
			m_shadowNeedsRender = true;
		}
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
	shader.setBool(formatAttribute("shadow"), m_shadow);
	shader.setMat4(formatAttribute("lightSpaceMatrix"), m_lightSpaceMatrix[0]);
	shader.setInt(formatAttribute("shadowMap"), m_shadowTextureSlot);
	shader.setFloat(formatAttribute("farPlane"), m_parameters.far_plane);
	shader.setInt(formatAttribute("type"), 0);
}

void DirectionalLight::calculateLightSpaceMatrix()
{
	m_lightSpaceMatrix[0] = glm::ortho(
		m_parameters.minx,
		m_parameters.maxx,
		m_parameters.miny,
		m_parameters.maxy,
		m_parameters.near_plane,
		m_parameters.far_plane) *
		glm::lookAt(
			m_parameters.directionalLightScale * glm::normalize(m_position), // "place" the light on a sphere with radius 2.5
			glm::vec3(0.0f), // looking at origin
			m_parameters.UP); // up vector is towards t
}