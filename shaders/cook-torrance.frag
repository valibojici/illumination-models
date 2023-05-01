@extends "base_shader.frag"

@section "material"
struct Material{
   vec3 albedo;     // "diffuse" color
   float roughness; // 0 = mirror | 1 = very rought
   vec3 f0;         // reflectance function at normal incidence (for fresnel)
   float ratio;     // => mix(diffuse, specular, ratio)
   vec3  ia;        // ambient intensity/color
   float ka;        // ambient coefficient
};
uniform Material u_material;
@endsection

@section "extra_uniforms"
uniform int u_Gindex = 0;
uniform int u_Dindex = 0;
@endsection

@section "BRDF_implementation"
// fresnel term
vec3 F_Schlick(vec3 f0, float VH);

// geometrical attenuation coefficient functions
// cook torrance paper version
float G_Cook(float VH, float NH, float NV, float NL);
// uncorrelated smith function for beckmann
float G2_U_Beckmann(float NV, float NL);
// uncorrelated smith function for ggx
float G2_U_GGX(float NV, float NL);
// height-correlated smith function for Beckmann
float G2_Beckmann(float NV, float NL);
// height-correlated smith function for GGX
float G2_GGX(float NV, float NL);

// slope distribution function
// beckmann version, used in cook-torrance paper
float D_Beckmann(float NH);
// ggx 
float D_GGX(float NH);
// phong
float D_Phong(float NH);

vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir){
    // calculate diffuse term

    // get the diffuse color from texture and do gamma correction
    vec3 diffuse;
    if(u_hasDiffTexture){
        diffuse = texture(u_DiffuseTex, fs_in.texCoords).rgb;
    } else {
        diffuse = u_gammaCorrect ? toLinear(u_material.albedo) : u_material.albedo;
    }

    // get the halfway direction vector between light direction and view direction
    vec3 halfway = normalize(lightDir + viewDir);

    // calculate vectors used in calculations
    float NH = dot(normal, halfway);
    float NL = dot(normal, lightDir);
    float VH = dot(viewDir, halfway);
    float NV = dot(normal, viewDir);
    
    // we need to calculate F D and G terms
    // F - Fresnel Term
    vec3 fresnel = F_Schlick(u_material.f0, VH);
    // D - Slope distribution function
    float slope_distribution;
    switch(u_Dindex){ // chosen from application side
        case 0: slope_distribution = D_Beckmann(NH); break;
        case 1: slope_distribution = D_GGX(NH); break;
        case 2: slope_distribution = D_Phong(NH);
    }
    // G - Geometrical attenuation function
    float geometrical_attenuation;
    switch(u_Gindex){ // chosen from application side
        case 0: geometrical_attenuation = G_Cook(VH, NH, NV, NL); break;
        case 1: geometrical_attenuation = G2_U_Beckmann(NV, NL); break;
        case 2: geometrical_attenuation = G2_U_GGX(NV, NL); break;
        case 3: geometrical_attenuation = G2_Beckmann(NV, NL); break;
        case 4: geometrical_attenuation = G2_GGX(NV, NL); break;
    }

    // formula is FDG / (4 * NL * NV)
    vec3 specular =  (fresnel * slope_distribution * geometrical_attenuation) / 
                    (4 * max(0.0001f, NL) * max(0.0001f, NV));
    
    specular = specular * PI; // multiply with PI to denormalize

    float ratio = u_hasMetallicTexture ? texture(u_MetallicTex, fs_in.texCoords).r : u_material.ratio;
    return mix(diffuse, specular, ratio); // the result depends on the diffuse/specular ratio
}

// fresnel term
vec3 F_Schlick(vec3 f0, float VH){
    return f0 + (1 - f0) * pow(1-VH, 5);
}

// geometrical attenuation coefficient functions
// cook torrance paper version
float G_Cook(float VH, float NH, float NV, float NL){
    return min(1, min(2 * NH * NV / NV, 2.0f * NH * NL / NV));
}
// smith version
float G2_U_Beckmann(float NV, float NL){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    
    // calculate a for V
    float a_V = NV / (alpha * sqrt(1-NV*NV));
    float g_V = 1.0f;
    if(a_V < 1.6f) {
        g_V = (3.535*a_V + 2.181*a_V*a_V) / (1 + 2.276 * a_V + 2.577 * a_V * a_V);
    }

    // calculate a for L
    float a_L = NL / (alpha * sqrt(1-NL*NL));
    float g_L = 1.0f;
    if(a_L < 1.6f) {
        g_L = (3.535*a_L + 2.181*a_L*a_L) / (1 + 2.276 * a_L + 2.577 * a_L * a_L);
    }
    
    return g_V * g_L;
}

float G2_Beckmann(float NV, float NL){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float a_V = NV / (alpha * sqrt(1-NV*NV));
    float lambda_V = a_V >= 1.6f ? 0.0f : (1 - 1.259*a_V + 0.396*a_V*a_V) / (3.535*a_V + 2.181 * a_V * a_V);

    float a_L = NL / (alpha * sqrt(1-NL*NL));
    float lambda_L = a_L >= 1.6f ? 0.0f : (1 - 1.259*a_L + 0.396*a_L*a_L) / (3.535*a_L + 2.181 * a_L * a_L);

    return 1 / (1 + lambda_L + lambda_V);
}

// GGX
float G2_U_GGX(float NV, float NL){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float sq_alpha = alpha * alpha;
    float g_V = 2 * NV / (NV + sqrt(NV*NV + sq_alpha * (1 - NV * NV)));
    float g_L = 2 * NL / (NL + sqrt(NL*NL + sq_alpha * (1 - NL * NL)));
    return g_V * g_L;
}

float G2_GGX(float NV, float NL){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float sq_alpha = alpha * alpha;

    return 2 * NL * NV / (NL * sqrt(sq_alpha + NV * (NV - alpha * NV)) + NV * sqrt(sq_alpha + NL * (NL - alpha * NL)));
}

// slope distribution function
// beckmann version, used in cook-torrance paper
float D_Beckmann(float NH){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float a = exp((NH * NH - 1) / (NH * NH * alpha * alpha));
    float b = pow(alpha, 2) * pow(NH, 4);
    return a / b;
}
// GGX distribution
float D_GGX(float NH){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    alpha = alpha * alpha;
    return alpha * alpha / (pow((pow(NH, 4) * (alpha * alpha - 1) + 1), 2));
}

// phong distribution
float D_Phong(float NH){
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    alpha = 2 / (alpha * alpha) - 2;
    return (alpha + 2) / 2 * pow(NH, alpha);
}
@endsection