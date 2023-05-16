#include "PointLight.h"

PointLight::PointLight(int index, const glm::vec3& position, const glm::vec3& color) : Light(index, color)
{
	m_type = Type::POINT;
	std::stringstream ss;
	ss << "Point light #" << index;
	m_name = ss.str();
	m_position = position;

	// set light projection parameters
	m_parameters.point(1.0f, 0.1f, 12.0f);
	calculateLightSpaceMatrix();
}

void PointLight::calculateLightSpaceMatrix()
{
	/* order is
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	*/
	const std::vector<glm::vec3> up_directions{
		{0.0f, -1.0f, 0.0f}, // positive x -> right
		{0.0f, -1.0f, 0.0f}, // negative x -> left
		{0.0f, 0.0f, 1.0f}, // positive y -> up
		{0.0f, 0.0f, -1.0f}, // negative y -> down
		{0.0f, -1.0f, 0.0f}, // positive z -> BACK
		{0.0f, -1.0f, 0.0f}, // negative z -> FRONT
	};

	const std::vector<glm::vec3> look_directions{
		{1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, -1.0f}
	};
	
	m_lightSpaceMatrix.clear();
	// go through every face
	for (int i = 0; i < 6; ++i) {
		m_lightSpaceMatrix.push_back(
			glm::perspective(
				glm::radians(m_parameters.fov), 
				m_parameters.aspect, 
				m_parameters.near_plane, 
				m_parameters.far_plane) *
			glm::lookAt(m_position, m_position + look_directions[i], up_directions[i])
		);
	}
}

void PointLight::imGuiRender()
{
	Light::imGuiRender();

	if (ImGui::DragFloat3("Position", &m_position.x, 0.01f)) {
		calculateLightSpaceMatrix();
		m_shadowNeedsRender = true;
	}
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
	shader.setInt(formatAttribute("shadowMapCube"), m_shadowTextureSlot);
	shader.setBool(formatAttribute("shadow"), m_shadow);
	shader.setFloat(formatAttribute("farPlane"), m_parameters.far_plane);
	shader.setInt(formatAttribute("type"), 2);
}
