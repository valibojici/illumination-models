#pragma once
#include "Material.h"
#include "glm/glm.hpp"
#include "imgui.h"

class CookTorranceMaterial : public Material
{
private:

	// "diffuse" color
	glm::vec3 m_albedo = glm::vec3(1.0f, 0.0f, 0.0f);

	// 0 = perfectly smooth, 1 = very rough
	// should be between these to avoid division errors
	float m_roughness = 0.5;
	
	// refraction index of material, used for calculating fresnel term
	float m_refractionIndex = 1.2f;

	// diffuse : specular ratio (1 = diffuse 0 = specular)
	float m_ratio = 0.5f;

	// reflectance function F at normal incidence (perpendicular)
	// used for metals because the value is RGB
	glm::vec3 m_f0 = glm::vec3(0.04f);

	int m_Gindex = 0;
	int m_Dindex = 0;

	// ambient color
	glm::vec3 m_ia = glm::vec3(1.0f, 0.0f, 0.0f);
	// ambient coefficient
	float m_ka = 0.001f;
public:
	/// <summary>
	/// Renders UI and does NOT set uniforms on value change
	/// </summary>
	void imGuiRender(Shader& shader);

	/// <summary>
	/// Sets all uniforms in shader
	/// </summary>
	void setUniforms(Shader& shader);

	void setColor(const glm::vec3& albedo) override { m_albedo = albedo; }
	void setAmbient(const glm::vec3& c) override { m_ia = c; }
	void disableHighlights() override { m_roughness = 0.999f; m_ratio = 0.05f; }
	void setRoughness(float r) { m_roughness = r; }
	void setAmbientCoefficient(const float c) { m_ka = c; }
};

