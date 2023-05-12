#include "CookTorranceMaterial.h"

void CookTorranceMaterial::imGuiRender()
{
	ImGui::Combo("Distribution function", &m_Dindex, "Beckmann\0GGX\0Phong\0\0");
	ImGui::Combo("Geometrical function", &m_Gindex, "Cook-Torrance\0Beckmann uncorrelated G2 Smith\0GGX uncorrelated G2 Smith\0Beckmann correlated G2 Smith\0GGX correlated G2 Smith\0\0");

	ImGui::Checkbox("Output DFG functions", &m_outputDFG);
	if (m_outputDFG) {
		ImGui::RadioButton("Output Slope distribution", &m_outputDFG_choice, 0);
		ImGui::RadioButton("Output Fresnel", &m_outputDFG_choice, 1);
		ImGui::RadioButton("Output Geometrical attenuation", &m_outputDFG_choice, 2);
	}

	ImGui::ColorEdit3("Albedo", &m_albedo[0], ImGuiColorEditFlags_Float);
	ImGui::DragFloat("Roughness", &m_roughness, 0.001f, 0.001f, 0.999f);
	ImGui::Checkbox("Use custom F0", &m_customF0);
	if (m_customF0) {
		ImGui::ColorEdit3("f0", &m_f0[0], ImGuiColorEditFlags_Float);
	}
	ImGui::DragFloat("Diffuse/Specular ratio", &m_ratio, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient intensity", &m_ka, 0.00005f, 0.0f, 0.5f, "%.5f");
	ImGui::ColorEdit3("Ambient color", &m_ia.x, ImGuiColorEditFlags_Float);
}

void CookTorranceMaterial::setUniforms(Shader& shader)
{
	shader.setVec3("u_material.albedo", m_albedo);
	shader.setFloat("u_material.roughness", m_roughness);
	shader.setVec3("u_material.f0", m_customF0 ? m_f0 : glm::vec3(0.0f));
	shader.setFloat("u_material.ratio", m_ratio);
	shader.setFloat("u_material.ka", m_ka);
	shader.setVec3("u_material.ia", m_ia);
	shader.setInt("u_Gindex", m_Gindex);
	shader.setInt("u_Dindex", m_Dindex);

	shader.setBool("u_outputDFG", m_outputDFG);
	shader.setInt("u_outputOnlyBRDF", m_outputDFG ? 1 : 0);
	shader.setInt("u_outputD", m_outputDFG_choice == 0 ? 1 : 0);
	shader.setInt("u_outputF", m_outputDFG_choice == 1 ? 1 : 0);
	shader.setInt("u_outputG", m_outputDFG_choice == 2 ? 1 : 0);
}
