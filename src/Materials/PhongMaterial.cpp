#include "PhongMaterial.h"

void PhongMaterial::imGuiRender(Shader& shader)
{
    ImGui::ColorEdit3("Diffuse color", &m_diffuseColor.x);

    if (ImGui::DragFloat("Kd", &m_kd, 0.001f, 0.0f, 1.0f)) {
        m_ks = std::min(m_ks, 1 - m_kd);
    }
    if (ImGui::DragFloat("Ks", &m_ks, 0.001f, 0.0f, 1.0f)) {
        m_kd = std::min(m_kd, 1 - m_ks);
    }

    ImGui::DragFloat("Alpha (shininess)", &m_alpha, 1.0f, 1.0f, 1024.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
    ImGui::DragFloat("Ambient intensity", &m_ka, 0.00005f, 0.0f, 0.5f, "%.5f");
    ImGui::ColorEdit3("Ambient color", &m_ia.x);
}

void PhongMaterial::setUniforms(Shader& shader)
{
    shader.setFloat("u_kd", m_kd);
    shader.setVec3("u_diffuseColor", m_diffuseColor);
    shader.setFloat("u_ks", m_ks);
    shader.setFloat("u_alpha", m_alpha);
    shader.setFloat("u_ka", m_ka);
    shader.setVec3("u_ia", m_ia);
}
