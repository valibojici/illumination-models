#include "PostprocessUI.h"

void PostprocessUI::addShaders(std::vector<Shader*> shaders)
{
    m_shaders.insert(m_shaders.end(), shaders.begin(), shaders.end());
}

void PostprocessUI::setUniforms() const
{
	for (auto& shader : m_shaders) {
		shader->bind();
		shader->setBool("u_gammaCorrect", m_gammaCorrect);
		shader->setInt("u_hdr", m_hdr);
        shader->setFloat("u_exposure", m_exposure);
        shader->setFloat("u_reinhardWhite", m_reinhardWhite);
	}
}

void PostprocessUI::onRenderImGui()
{
    if (ImGui::Checkbox("Enable gamma correction", &m_gammaCorrect)) {
        setUniforms();
    }

    if (ImGui::Combo("HDR", &m_hdr, "Disabled\0Reinhard tonemap\0Filmic tonemap\0\0")) {
        setUniforms();
    }

    if (m_hdr == 1 && ImGui::DragFloat("Reinhard L_White", &m_reinhardWhite, 0.01f, 0.01f, 100.0f, "%.3f")) {
        setUniforms();
    }

    if (ImGui::DragFloat("Exposure", &m_exposure, 0.001f, -5.0f, 5.0f)) {
        setUniforms();
    }
}
