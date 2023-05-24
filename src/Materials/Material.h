#pragma once
#include "Shader.h"

/// <summary>
/// Abstract class for a material
/// </summary>
class Material
{
protected:
	int m_presetIndex = 0;

	// flag for presets in UI
	bool m_showPresetsUI = false;
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

	// set albedo/diffuse
	virtual void setColor(const glm::vec3& color) = 0;
	
	// set ambient color
	virtual void setAmbient(const glm::vec3& color) = 0;

	// use very high exponent for phong, or low roughness
	virtual void disableHighlights() = 0;

	void setShowPresetsUI(bool value) { m_showPresetsUI = value; }
};

