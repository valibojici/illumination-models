#pragma once
#include "Shader.h"
#include "imgui.h"

/// <summary>
/// Helper class for postprocessing (gamma correction, hdr).
/// </summary>
class PostprocessUI
{
private:
	// shaders that need uniforms for postprocessing
	std::vector<Shader*> m_shaders;
	
	// flag to enable/disable gamma correction
	bool m_gammaCorrect = true;

	// flag to use/not use HDR
	bool m_hdr = true;

	// parameter for Reinhard tonemapping
	float m_reinhardWhite = 4;
public:
	/// <summary>
	/// Used to "register" shaders so postprocessing uniforms can be set for all these shaders 
	/// </summary>
	/// <param name="shaders"> : shaders that need uniforms for postprocessing</param>
	void addShaders(std::vector<Shader*> shaders);

	/// <summary>
	/// Sets all uniforms, for each registered shader
	/// </summary>
	void setUniforms() const;

	/// <summary>
	/// Render ImGui UI for postprocessing effects
	/// </summary>
	void onRenderImGui();

	inline void setGammaCorrection(bool value) { m_gammaCorrect = value; }
	inline bool getGammaCorrection()const { return m_gammaCorrect; }
	inline void setHDR(bool value) { m_hdr = value; }
};

