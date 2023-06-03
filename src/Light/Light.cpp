#include "Light.h"

std::unique_ptr<Mesh> Light::s_lightMesh;

Light::Light(int index, const glm::vec3& color) : m_index(index), m_color(color)
{
	// set static variable if it has not been set
	if (!s_lightMesh) {
		s_lightMesh = std::unique_ptr<Mesh>(Mesh::getCube(0.1f, 0.1f, 0.1f));
	}
}

void Light::setViewProjectionParameters(const ViewProjectionParameters& param)
{
	m_parameters = param;
	calculateLightSpaceMatrix();
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

void Light::imGuiRender()
{
	ImGui::Checkbox("Draw light", &m_draw); // draw cube or not
	ImGui::SameLine();
	ImGui::Checkbox("Enable", &m_enabled); // enable/disable
	ImGui::SameLine();
	ImGui::Checkbox("Shadows", &m_shadow);
	ImGui::ColorEdit3("Light Color", &m_color.x, ImGuiColorEditFlags_Float);
	ImGui::DragFloat("Light intensity", &m_intensity, 0.01f, 0.0f, 10.0f);
}

std::string Light::formatAttribute(const std::string& name)
{
	std::stringstream ss;
	ss << "u_lights[" << m_index << "]." << name;
	return ss.str();
}

Light::ViewProjectionParameters& Light::ViewProjectionParameters::directional(
	float minx_, float maxx_, float miny_, float maxy_, float near_plane_, float far_plane_, float scale_, glm::vec3 UP_) {

	minx = minx_;
	maxx = maxx_;
	miny = miny_;
	maxy = maxy_;
	near_plane = near_plane_;
	far_plane = far_plane_;
	directionalLightScale = scale_;
	UP = UP_;
	return *this;
}

Light::ViewProjectionParameters& Light::ViewProjectionParameters::spotlight(float aspect_, float near_plane_, float far_plane_, glm::vec3 UP_) {
	aspect = aspect_;
	near_plane = near_plane_;
	far_plane = far_plane_;
	UP = UP_;
	return *this;
}

Light::ViewProjectionParameters& Light::ViewProjectionParameters::point(float aspect_, float near_plane_, float far_plane_) {
	fov = 90.0f;
	spotlight(aspect_, near_plane_, far_plane_, glm::vec3(0.0f));
	return *this;
}