#pragma once
#include "Light/Light.h"

/// <summary>
/// Class for a spot light
/// </summary>
class Spotlight : public Light
{
private:
	/// <summary>
	/// target of spot light in world space
	/// </summary>
	glm::vec3 m_target;

	/// <summary>
	/// attenuation factors: constant, linear, quadratic
	/// </summary>
	glm::vec3 m_attenuation = glm::vec3(1.0f, 0.1f, 0.04f);

	/// <summary>
	/// Cut off angle, in degrees.
	/// The cos value is passed to the shader
	/// </summary>
	float m_cutOff = 30;

	/// <summary>
	/// Outer cut off angle, in degrees.
	/// The cos value is passed to the shader
	/// </summary>
	float m_outerCutOff = 35;
public:

	/// <summary>
	/// Constructs a spot light
	/// </summary>
	/// <param name="index">: index of light in the *shader uniform array*</param>
	/// <param name="position">: position of the light</param>
	/// <param name="target">: where the spotlight is pointing</param>
	Spotlight(int index, const glm::vec3& position, const glm::vec3& target);

	/// <summary>
	/// Draw UI in ImGui, sets uniforms on value change.
	/// </summary>
	void imGuiRender(Shader& shader) override;

	/// <summary>
	/// Sets all uniforms for this light (at m_index)
	/// </summary>
	void setUniforms(Shader& shader) override;

	float getCutoff() const { return m_cutOff; }
	float getOuterCutoff() const { return m_outerCutOff; }
	glm::vec3 getTarget() const { return m_target; }
};

