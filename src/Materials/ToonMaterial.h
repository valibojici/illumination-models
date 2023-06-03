#pragma once
#include "Material.h"
#include "glm/glm.hpp"
#include "imgui.h"

/// <summary>
/// Class for a Toon material
/// </summary>
class ToonMaterial : public Material
{
protected:
	// diffuse color, default red
	glm::vec3 m_diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// ambient color
	glm::vec3 m_ambientColor = glm::vec3(0.3f, 0.0f, 0.0f);

	// ambient factor
	float m_ambientFactor = 0.4f;

public:
	void setColor(const glm::vec3& color) override { m_diffuseColor = color; }
	void setAmbient(const glm::vec3& color) override { m_ambientColor = color; }
	void disableHighlights() override {}

	/// <summary>
	/// Renders UI in ImGui, does not set uniforms
	/// </summary>
	virtual void imGuiRender() override;

	/// <summary>
	/// Sets all material uniforms in shader
	/// </summary>
	virtual void setUniforms(Shader& shader) override;
};

