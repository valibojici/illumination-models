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
	virtual void imGuiRender(Shader& shader) = 0;

	/// <summary>
	/// Sets all uniforms in shader
	/// </summary>
	virtual void setUniforms(Shader& shader) = 0;
	virtual ~Material() {}
};

