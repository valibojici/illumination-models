#include "Light.h"

std::unique_ptr<Mesh> Light::s_lightMesh;

Light::Light(int index) : m_index(index)
{
	// set static variable if it has not been set
	if (!s_lightMesh) {
		s_lightMesh = std::unique_ptr<Mesh>(Mesh::getSphere(0.05f, 10));
	}
}

void Light::setPosition(const glm::vec3& pos)
{
	m_position = pos;
}

void Light::draw(Shader& shader)
{
	// skip drawing if m_draw is true
	if (!m_draw) {
		return;
	}

	shader.setVec3("u_emission", m_color);
	shader.setFloat("u_ka", 0.0f);
	shader.setMat4("u_modelMatrix", m_modelMatrix * glm::translate(m_position) );
	s_lightMesh->draw(shader);
	// reset emission on exit
	shader.setVec3("u_emission", glm::vec3(0.0f));
}

void Light::imGuiRender(Shader& shader)
{
	ImGui::Checkbox("Draw light", &m_draw);
	ImGui::SameLine();
	ImGui::Checkbox("Enable", &m_enabled);
	ImGui::ColorEdit3("Light Color", &m_color.x, ImGuiColorEditFlags_Float);
	ImGui::DragFloat("Light intensity", &m_intensity, 0.01f, 0.0f, 10.0f);
}

std::string Light::formatAttribute(const std::string& name)
{
	std::stringstream ss;
	ss << "u_lights[" << m_index << "]." << name;
	return ss.str();
}
