@extends "base_shader.frag"

@section "material"
struct Material{
   vec3 kd;            // diffuse coefficient
   vec3  diffuseColor; // diffuse color
   vec3 ks;            // specular coefficient
   float alpha;        // shininess
   float ka;           // ambient coefficient
   vec3  ia;           // ambient intensity/color
};
uniform Material u_material;
@endsection

@section "extra_uniforms"
// true if specular should NOT be divided by geometry term
uniform bool u_modifiedSpecular;
@endsection

@section "BRDF_implementation"
vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir){
    // calculate diffuse term

    // get the diffuse color from texture and do gamma correction
    vec3 diffuseCol = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_material.diffuseColor;
    diffuseCol = u_gammaCorrect ? toLinear(diffuseCol) : diffuseCol;
    vec3 diffuse = u_material.kd * diffuseCol;

    // calculate specular term
    // get the halfway direction vector between light direction and view direction
    vec3 halfway = normalize(lightDir + viewDir);
    // get the angle between the normal and halfway direction
    float cosPhi = max(0.0f, dot(normal, halfway));
    // calculate/ get the specular coefficient (from texture)
    vec3 specFactor = u_hasSpecTexture ? texture(u_SpecularTex, fs_in.texCoords).rgb : u_material.ks;
    // calculate shininess from the roughness 
    float alpha = u_hasRoughTexture ? 2 * pow(texture(u_RoughTex, fs_in.texCoords).r, -2) : u_material.alpha;
    
    vec3 specular = specFactor * pow(cosPhi, alpha);
    if(!u_modifiedSpecular){
        specular /= max(0.0001f, geometryTerm);
    }
    return diffuse + specular;
}
@endsection