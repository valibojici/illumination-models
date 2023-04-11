#pragma once
#include "Material.h"
#include "glm/glm.hpp"
#include "imgui.h"

/// <summary>
/// Class for a Phong material
/// </summary>
class PhongMaterial : public Material
{
protected:
	// diffuse color, default red
	glm::vec3 m_diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);

	// diffuse coefficient
	glm::vec3 m_kd = glm::vec3(0.5f);

	// specular coefficient
	glm::vec3 m_ks = glm::vec3(0.5f);

	// ambient coefficient
	float m_ka = 0.002f;

	// ambient color
	glm::vec3 m_ia = glm::vec3(1.0f, 0.0f, 0.0f);

	// shininess
	float m_alpha = 32.0f;

	// flag to tell the shader to not divide specular by geometry term
	bool m_modifiedSpecular = false;
public:
	void setDiffuseColor(const glm::vec3& color) { m_diffuseColor = color; }
	void setDiffuseCoefficient(const glm::vec3& kd) { m_kd = kd; }
	void setSpecularCoefficient(const glm::vec3& ks) { m_ks = ks; }
	void setAmbientCoefficient(float ka) { m_ka = ka; }
	void setShininess(float alpha) { m_alpha = alpha; }
	void setAmbientColor(const glm::vec3& color) { m_ia = color; }

	/// <summary>
	/// Renders UI in ImGui, does not set uniforms
	/// </summary>
	virtual void imGuiRender(Shader& shader) override;

	/// <summary>
	/// Sets all material uniforms in shader
	/// </summary>
	virtual void setUniforms(Shader& shader) override;
};

