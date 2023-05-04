#pragma once
#include "Light.h"

/// <summary>
/// Class for a point light
/// </summary>
class PointLight : public Light
{
private:
	/// <summary>
	/// attenuation factors: constant, linear, quadratic
	/// </summary>
	glm::vec3 m_attenuation = glm::vec3(1.0f, 0.1f, 0.04f);
public:
	/// <summary>
	/// Constructs a new point light derived from Light class
	/// </summary>
	/// <param name="index">: index of light in the *shader uniform array*</param>
	/// <param name="position">: position of the light</param>
	PointLight(int index, const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f));

	void calculateLightSpaceMatrix() override;

	/// <summary>
	/// Draw UI in ImGui
	/// </summary>
	void imGuiRender() override;

	/// <summary>
	/// Sets all uniforms for this light (at m_index)
	/// </summary>
	void setUniforms(Shader& shader) override;
};

