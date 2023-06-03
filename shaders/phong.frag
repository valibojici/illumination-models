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
// true if specular term is NOT be divided by geometry term
uniform bool u_modifiedSpecular;
@endsection

@section "BRDF_implementation"
// Phong BRDF function
vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir){

    vec3 diffuseCol;
    if(u_hasDiffTexture){
        // get the diffuse color from texture and do gamma correction
        diffuseCol = texture(u_DiffuseTex, fs_in.texCoords).rgb;
    } else {
        diffuseCol = u_gammaCorrect ? toLinear(u_material.diffuseColor) : u_material.diffuseColor;
    }
    // calculate diffuse term
    vec3 diffuse = u_material.kd * diffuseCol;

    // get the reflected direction of the light using the normal
    vec3 reflectDir = reflect(-lightDir, normal);
    // get the angle between the reflection and viewing direction
    float cosPhi = max(0.0f, dot(viewDir, reflectDir));
    // calculate/ get the specular coefficient (from texture)
    vec3 specFactor = u_hasSpecTexture ? texture(u_SpecularTex, fs_in.texCoords).rgb : u_material.ks;
    // approximate shininess from the roughness texture if it is used
    float alpha = u_hasRoughTexture ? 2 * pow(texture(u_RoughTex, fs_in.texCoords).r, -2) : u_material.alpha;
    
    // calculate specular term
    vec3 specular = specFactor * pow(cosPhi, alpha);
    // by default specular term is divided by geometryTerm => geometryTerm has no impact on specular part
    if(!u_modifiedSpecular){
        specular /= max(0.0001f, geometryTerm);
    }
    
    return diffuse + specular;
}
@endsection