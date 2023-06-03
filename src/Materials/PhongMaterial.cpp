#include "PhongMaterial.h"
#include "Scene/Scene.h"

void PhongMaterial::imGuiRender()
{
	if (m_showPresetsUI && ImGui::Combo("Material", &m_presetIndex, "Custom\0Gold\0Copper\0Iron\0\0")) {
		switch (m_presetIndex)
		{
		case 1:
			m_diffuseColor = { 0.7f, 0.45f, 0.0f };
			m_kd = glm::vec3(0.02f);
			m_ks = { 1.0f, 0.67f, 0.1f };
			break;
		case 2:
			m_diffuseColor = { 0.45f, 0.24f, 0.21f };
			m_kd = glm::vec3(0.05f);
			m_ks = { 1.0f, 0.5f, 0.35f };
			break;
		case 3:
			m_diffuseColor = glm::vec3(0.2f);
			m_kd = glm::vec3(0.15f);
			m_ks = glm::vec3(1.0f);
			break;
		default:
			break;
		}
		if (m_presetIndex != 0) {
			m_alpha = 68;
			m_ka = 0.001f;
			m_ia = m_diffuseColor;
		}
	}
	if (m_presetIndex != 0)return;
    ImGui::ColorEdit3("Diffuse color", &m_diffuseColor.x, ImGuiColorEditFlags_Float);

    static bool setAllDiffuse = true;
    if (ImGui::DragFloat3("Diffuse coefficient", &m_kd[0], 0.001f, 0.0f, 1.0f)) {
        if(setAllDiffuse) m_kd[1] = m_kd[2] = m_kd[0];
    }
    ImGui::Checkbox("Set all##diffuse", &setAllDiffuse);

    static bool setAllSpecular = true;
    if (ImGui::DragFloat3("Specular coefficient", &m_ks[0], 0.001f, 0.0f, 1.0f)) {
        if(setAllSpecular) m_ks[1] = m_ks[2] = m_ks[0];
    }
    ImGui::Checkbox("Set all##specular", &setAllSpecular);

    ImGui::Checkbox("Do not divide specular by geometry term", &m_modifiedSpecular);
    ImGui::SameLine();
    Scene::helpPoput("If the specular term is not divided by the geometry term it means\
 that the geometry term has impact");

    ImGui::DragFloat("Alpha (shininess)", &m_alpha, 1.0f, 1.0f, 1024.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
    ImGui::DragFloat("Ambient intensity", &m_ka, 0.00005f, 0.0f, 0.5f, "%.5f");
    ImGui::ColorEdit3("Ambient color", &m_ia.x, ImGuiColorEditFlags_Float);
}

void PhongMaterial::setUniforms(Shader& shader)
{
    shader.setVec3("u_material.kd", m_kd);
    shader.setVec3("u_material.diffuseColor", m_diffuseColor);
    shader.setVec3("u_material.ks", m_ks);
    shader.setFloat("u_material.alpha", m_alpha);
    shader.setFloat("u_material.ka", m_ka);
    shader.setVec3("u_material.ia", m_ia);
    shader.setBool("u_modifiedSpecular", m_modifiedSpecular);
}

void PhongMaterial::defaultParameters()
{
	m_presetIndex = 0;
	m_diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);
	m_kd = glm::vec3(1.0f);
	m_ks = glm::vec3(0.5f);
	m_ka = 0.005f;
	m_ia = glm::vec3(1.0f, 0.0f, 0.0f);
	m_alpha = 32.0f;
	m_modifiedSpecular = false;
}
