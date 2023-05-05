#include "ToonMaterial.h"

void ToonMaterial::imGuiRender()
{
    ImGui::ColorEdit3("Diffuse color", &m_diffuseColor.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Ambient color", &m_ambientColor.x, ImGuiColorEditFlags_Float);
    ImGui::DragFloat("Ambient intensity", &m_ambientFactor, 0.001f);
}

void ToonMaterial::setUniforms(Shader& shader)
{
    shader.setVec3("u_material.diffuseColor", m_diffuseColor);
    shader.setVec3("u_material.ambientColor", m_ambientFactor * m_ambientColor);
}
