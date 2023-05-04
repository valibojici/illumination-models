#pragma once
#include "Shader.h"

/// <summary>
/// Abstract class for a material
/// </summary>
class Material
{
public:
	/// <summary>
	/// Renders UI and does NOT set uniforms on value change
	/// </summary>
	virtual void imGuiRender() = 0;

	/// <summary>
	/// Sets all uniforms in shader
	/// </summary>
	virtual void setUniforms(Shader& shader) = 0;
	virtual ~Material() {}

	virtual void setColor(const glm::vec3& color) = 0;
	virtual void setAmbient(const glm::vec3& color) = 0;
	virtual void disableHighlights() = 0;
};

