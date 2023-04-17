#include "PhongMaterial.h"
#include "Scene/Scene.h"

void PhongMaterial::imGuiRender(Shader& shader)
{
    ImGui::ColorEdit3("Diffuse color", &m_diffuseColor.x, ImGuiColorEditFlags_Float);

    static bool setAllDiffuse = true;
    if (ImGui::DragFloat3("Diffuse coefficient", &m_kd[0], 0.001f, 0.0f, 1.0f)) {
        if(setAllDiffuse) m_kd[1] = m_kd[2] = m_kd[0];
    }
    ImGui::Checkbox("Set all##diffuse", &setAllDiffuse);

    static bool setAllSpecular = true;
    if (ImGui::DragFloat3("Specular coefficient", &m_ks[0], 0.001f, 0.0f, 1.0f)) {
        if(setAllSpecular) m_ks[1] = m_ks[2] = m_ks[0];
    }
    ImGui::Checkbox("Set all##specular", &setAllSpecular);

    ImGui::Checkbox("Do not divide specular by geometry term", &m_modifiedSpecular);
    ImGui::SameLine();
    Scene::helpPoput("If the specular term is divided by the geometry term it means\
 that the geometry term has no impact, this is a problem at grazing\
 angles but its more realistic because it is a basic fresnel effect");

    ImGui::DragFloat("Alpha (shininess)", &m_alpha, 1.0f, 1.0f, 1024.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
    ImGui::DragFloat("Ambient intensity", &m_ka, 0.00005f, 0.0f, 0.5f, "%.5f");
    ImGui::ColorEdit3("Ambient color", &m_ia.x, ImGuiColorEditFlags_Float);
}

void PhongMaterial::setUniforms(Shader& shader)
{
    shader.setVec3("u_material.kd", m_kd);
    shader.setVec3("u_material.diffuseColor", m_diffuseColor);
    shader.setVec3("u_material.ks", m_ks);
    shader.setFloat("u_material.alpha", m_alpha);
    shader.setFloat("u_material.ka", m_ka);
    shader.setVec3("u_material.ia", m_ia);
    shader.setBool("u_modifiedSpecular", m_modifiedSpecular);
}
