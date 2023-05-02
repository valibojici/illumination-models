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
public:
	struct ViewProjectionParameters {
		float minx = 0;
		float miny = 0;
		float maxx = 0;
		float maxy = 0;
		float fov = 0;
		float aspect = 1.0f;
		float near_plane = 0.1f;
		float far_plane = 5.0f;
		float directionalLightScale = 2.5f;
		glm::vec3 UP = { 0.0f, 0.0f, 1.0f };
		void directional(float minx_, float maxx_, float miny_, float maxy_, float near_plane_, float far_plane_, float scale_, glm::vec3 UP_) {
			minx = minx_;
			maxx = maxx_;
			miny = miny_;
			maxy = maxy_;
			near_plane = near_plane_;
			far_plane = far_plane_;
			directionalLightScale = scale_;
			UP = UP_;
		}
		void spotlight(float aspect_, float near_plane_, float far_plane_, glm::vec3 UP_) {
			aspect = aspect_;
			near_plane = near_plane_;
			far_plane = far_plane_;
			UP = UP_;
		}
		void point(float aspect_, float near_plane_, float far_plane_) {
			fov = 90.0f;
			spotlight(aspect_, near_plane_, far_plane_, glm::vec3(0.0f));
		}
	};

	enum class Type {
		POINT, DIRECTIONAL, SPOT
	};

protected:
	/// type of light
	Type m_type;

	// flag if the light position has been updated, used to render the scene for shadowmapping
	bool m_shadowNeedsRender = true;

	// parameters for shadowmapping (view and projection matrices)
	ViewProjectionParameters m_parameters;

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

	bool m_shadow = false;
	std::vector<glm::mat4> m_lightSpaceMatrix = { glm::mat4(1.0f) };

	/// <summary>
	/// Slot of the texture depth map (0-16)
	/// </summary>
	unsigned int m_shadowTextureSlot = 0;

	/// <summary>
	/// Helper function to get the name of attribute as a uniform name
	/// e.g. formatAttribute("position") == "u_lights[m_index].position"
	/// </summary>
	/// <param name="name">: name of attribute, same as in shader</param>
	std::string formatAttribute(const std::string& name);

	virtual void calculateLightSpaceMatrix() = 0;
public:
	/// <summary>
	///	The index refers to the index in the shader lights uniform array
	/// </summary>
	/// <param name="index">: index of the light in uniform array</param>
	Light(int index);

	// name of light displayed in UI: "<type> #<m_index>"
	std::string m_name;

	Type getType() const { return m_type; }

	/// <summary>
	/// Return flag to check if re rendering the scene for shadow mapping is necessary
	/// </summary>
	bool getShadowNeedsRender() const { return m_shadowNeedsRender; }

	/// <summary>
	/// Resets shadowNeedsRender flag (call after rendering scene)
	/// </summary>
	void resetShadowNeedsRender() { m_shadowNeedsRender = false; }

	/// <summary>
	/// Set if the light is casting shadow
	/// </summary>
	void setShadow(bool value) { m_shadow = true; }

	bool getShadow() const { return m_shadow; }

	unsigned int getShadowTextureSlot() const { return m_shadowTextureSlot; }
	void setShadowTextureSlot(unsigned int slot) { m_shadowTextureSlot = slot; }

	/// <summary>
	/// Sets the parameters which are used to generate lightspace matrix
	/// </summary>
	void setViewProjectionParameters(const ViewProjectionParameters& param);

	/// <summary>
	/// Get the light space matrices (only 1 for spot/directional)
	/// </summary>
	const std::vector<glm::mat4>& getLightSpaceMatrix() { return m_lightSpaceMatrix; }

	float getFarPlane() const { return m_parameters.far_plane; }

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
	virtual void imGuiRender();


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