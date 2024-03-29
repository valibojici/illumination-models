#pragma once
#include "Light.h"

/// <summary>
/// Class for a directional light
/// </summary>
class DirectionalLight : public Light
{
public:
	/// <summary>
	/// Constructs a new directional light derived from Light class
	/// </summary>
	/// <param name="index">: index of light in the *shader uniform array*</param>
	/// <param name="direction">: the light will be emitted from this direction</param>
	DirectionalLight(int index, const glm::vec3& direction, const glm::vec3& color = glm::vec3(1.0f));

	/// <summary>
	/// Calculate the light space matrix using the member parameters
	/// </summary>
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

