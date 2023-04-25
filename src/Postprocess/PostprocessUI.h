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

	// 0 = no hdr | 1 = Reinhard tonemapping | 2 = filmic tonemapping
	int m_hdr = 0;

	float m_exposure = 0;

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
	inline void setHDR(int value) { m_hdr = value; }
};
