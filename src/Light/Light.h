#pragma once
#include "imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "Shader.h"
#include "Mesh.h"

/// <summary>
/// Base class for lights. Must implement *setUniforms* and *setShaderPosition*
/// </summary>
class Light
{
protected:
	// static mesh for drawing a light source
	static std::unique_ptr<Mesh> s_lightMesh;

	// index of current light in the shader lights array
	int m_index;

	// default light color is white
	glm::vec3 m_color = glm::vec3(1.0f);

	// default light intensity is 1
	float m_intensity = 1.0f;

	// default light position is (0.0f, 1.0f, 0.0f) 
	glm::vec3 m_position = glm::vec3(0.0f, 1.0f, 0.0f);

	// model matrix for transformations, default mat4(1.0f)
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	// flag to draw light souce mesh
	bool m_draw = true;

	// flag to "turn off/on" light
	bool m_enabled = true;

	/// <summary>
	/// Helper function to get the name of attribute as a uniform name
	/// e.g. formatAttribute("position") == "u_lights[m_index].position"
	/// </summary>
	/// <param name="name">: name of attribute, same as in shader</param>
	std::string formatAttribute(const std::string& name);
public:
	/// <summary>
	///	The index refers to the index in the shader lights uniform array
	/// </summary>
	/// <param name="index">: index of the light in uniform array</param>
	Light(int index);

	// name of light displayed in UI: "<type> #<m_index>"
	std::string m_name;

	/// <summary>
	/// Abstract method that sets all uniforms for this light (at m_index)
	/// Must set the following uniforms: 
	/// [vec4 position (w == 0 for directional light, see setShaderPosition)]
	/// [float intensity]
	/// [vec3 color]
	/// [bool enabled]
	/// [vec3 attenuation (constant, linear, quadratic)]
	/// [vec3 target]
	/// [float cutOff (cos value, 1 if NOT spotlight)]
	/// [float outerCutOff (cos value)]
	/// </summary>
	virtual void setUniforms(Shader& shader) = 0;
	
	/// <summary>
	/// Draws the light mesh (if m_draw is true)
	/// </summary>
	virtual void draw(Shader& shader);

	/// <summary>
	/// Draws the GUI in ImGui for: [draw light] [enabled] [color] [intensity]
	/// Other options are in derived classes
	/// </summary>
	virtual void imGuiRender(Shader& shader);


	/// <summary>
	/// Set the position *member variable*
	/// </summary>
	void setPosition(const glm::vec3& pos);

	/// <summary>
	/// Get light position (not world space)
	/// </summary>
	glm::vec3 getPosition() const { return m_position; }

	/// <summary>
	/// Set the model matrix for transformations
	/// </summary>
	void setModelMatrix(const glm::mat4& model) { m_modelMatrix = model; }
};

