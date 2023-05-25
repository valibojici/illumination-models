@extends "base_shader.frag"

@section "material"
struct Material{
   vec3 albedo;     // "diffuse" color
   float roughness; // 0 = mirror | 1 = very rought
   vec3 f0;         // reflectance function at normal incidence (for fresnel)
   float metallic;     // => mix(diffuse, specular, ratio)
   vec3  ia;        // ambient intensity/color
   float ka;        // ambient coefficient
};
uniform Material u_material;
@endsection

@section "extra_uniforms"
uniform int u_Gindex = 0; // which geometry function is used
uniform int u_Dindex = 0; // which microfacet normal distribution is used

uniform bool u_outputDFG = false; // flag if should output N G or F functions to debug/view
uniform int u_outputD = 0; // output only D function
uniform int u_outputF = 0; // output only G function
uniform int u_outputG = 0; // output only F function
@endsection

@section "BRDF_implementation"
// calculates fresnel term using Shlick approximation
vec3 F_Schlick(vec3 f0, float VH);

// Geometrical attenuation functions - G
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

// slope distribution function - D
// beckmann version, used in cook-torrance paper
float D_Beckmann(float NH);
// ggx version
float D_GGX(float NH);
// phong version
float D_Phong(float NH);

vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir){
    // calculate diffuse/albedo term

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
    float NH = max(dot(normal, halfway), 0.0001f);
    float NL = max(dot(normal, lightDir), 0.0001f);
    float VH = max(dot(viewDir, halfway), 0.0001f);
    float NV = max(dot(normal, viewDir), 0.0001f);
    
    // calculate F D and G terms
    // F - Fresnel Term
    // gamma correct custom rgb f0 
    vec3 F0 = u_gammaCorrect ? toLinear(u_material.f0) : u_material.f0;
    // if f0 == 0 => dont use custom f0, use average value of 0.04 for non metals
    // and combine color with albedo based on metalness (more metalness == more albedo color for f0)
    if(u_material.f0 == vec3(0.0f)){
        F0 = mix(vec3(0.04f), diffuse, u_material.metallic);
    }
    // get fresnel term using this F0 
    vec3 fresnel = F_Schlick(F0, VH);


    // D - Slope distribution function
    float slope_distribution;
    // distribution function is chosen from UI
    switch(u_Dindex){ 
        case 0: slope_distribution = D_Beckmann(NH); break;
        case 1: slope_distribution = D_GGX(NH); break;
        case 2: slope_distribution = D_Phong(NH);
    }

    // G - Geometrical attenuation function
    float geometrical_attenuation;
    // Geometrical function is chosen from UI
    switch(u_Gindex){
        case 0: geometrical_attenuation = G_Cook(VH, NH, NV, NL); break;
        case 1: geometrical_attenuation = G2_U_Beckmann(NV, NL); break;
        case 2: geometrical_attenuation = G2_U_GGX(NV, NL); break;
        case 3: geometrical_attenuation = G2_Beckmann(NV, NL); break;
        case 4: geometrical_attenuation = G2_GGX(NV, NL); break;
    }

    // if flag is set => ouput only F D or G function
    if(u_outputDFG){
        vec3 result = vec3(0.0f);
        result += u_outputD * vec3(slope_distribution);
        result += u_outputG * vec3(geometrical_attenuation);
        result += u_outputF * fresnel;
        return result;
    }

    // formula is FDG / (4 * NL * NV) from cook-torrance paper
    vec3 specular =  (fresnel * slope_distribution * geometrical_attenuation) / (4 * NL * NV);

    // get metallic ratio
    float metallic = u_hasMetallicTexture ? texture(u_MetallicTex, fs_in.texCoords).r : u_material.metallic;
    // use at least 0.005 to have some fresnel reflections
    metallic = max(metallic, 0.005); 
    
    // linear interpolation between diffuse and specular based on metalness
    return mix(diffuse, specular, metallic); 
}

// uses Schlick approximation from "An Inexpensive BRDF Model for Physically-based Rendering"
vec3 F_Schlick(vec3 f0, float VH){
    return f0 + (1 - f0) * pow(1 - VH, 5);
}

// geometrical attenuation functions
// cook torrance paper version
float G_Cook(float VH, float NH, float NV, float NL){
    return min(1, min(2 * NH * NV / NV, 2.0f * NH * NL / NV));
}

// G2 smith uncorrelated, Beckmann distribution using approximation
float G2_U_Beckmann(float NV, float NL){
    // uncorrelated G2 function = G1(V) * G1(L)

    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;

    // compute G1(V) and G1(L):
    
    // calculate 'a' parameter for V
    float a_V = NV / (alpha * sqrt(1-NV*NV));
    float G1_V = 1.0f;
    if(a_V < 1.6f) {
        G1_V = (3.535*a_V + 2.181*a_V*a_V) / (1 + 2.276 * a_V + 2.577 * a_V * a_V);
    }

    // calculate 'a' parameter for V
    float a_L = NL / (alpha * sqrt(1-NL*NL));
    float G1_L = 1.0f;
    if(a_L < 1.6f) {
        G1_L = (3.535*a_L + 2.181*a_L*a_L) / (1 + 2.276 * a_L + 2.577 * a_L * a_L);
    }
    
    return G1_V * G1_L; // G1(V) * G1(L)
}

// G2 smith height correlated, Beckmann distribution using approximation
float G2_Beckmann(float NV, float NL){
    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;

    // calculate 'a' and Lambda(a) values for L and V
    float a_V = NV / (alpha * sqrt(1-NV*NV));
    float lambda_V = a_V >= 1.6f ? 0.0f : (1 - 1.259*a_V + 0.396*a_V*a_V) / (3.535*a_V + 2.181 * a_V * a_V);

    float a_L = NL / (alpha * sqrt(1-NL*NL));
    float lambda_L = a_L >= 1.6f ? 0.0f : (1 - 1.259*a_L + 0.396*a_L*a_L) / (3.535*a_L + 2.181 * a_L * a_L);

    return 1 / (1 + lambda_L + lambda_V);
}

// G2 smith uncorrelated, GGX distribution
float G2_U_GGX(float NV, float NL){
    // uncorrelated G2 function = G1(V) * G1(L)
    
    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float sq_alpha = alpha * alpha; // square value to appear more linear
    // compute G1(V) and G1(L)
    float G1_V = 2 * NV / (NV + sqrt(NV*NV + sq_alpha * (1 - NV * NV)));
    float G1_L = 2 * NL / (NL + sqrt(NL*NL + sq_alpha * (1 - NL * NL)));
    return G1_V * G1_L;
}

// G2 smith height correlated, GGX distribution
float G2_GGX(float NV, float NL){
    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float sq_alpha = alpha * alpha; // square value to appear more linear
    // use compact formula after substitutions and calculations
    return 2 * NL * NV / (NL * sqrt(sq_alpha + NV * (NV - alpha * NV)) + NV * sqrt(sq_alpha + NL * (NL - alpha * NL)));
}

// slope distribution function
// beckmann version, used in cook-torrance paper
float D_Beckmann(float NH){
    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    float a = exp((NH * NH - 1) / (NH * NH * alpha * alpha));
    float b = pow(alpha, 2) * pow(NH, 4);
    return a / b; // dont divide by PI to ignore normalization
}
// GGX distribution
float D_GGX(float NH){
    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    alpha = alpha * alpha;
    return alpha * alpha / (pow((pow(NH, 4) * (alpha * alpha - 1) + 1), 2)); // dont divide by PI to ignore normalization
}

// phong distribution
float D_Phong(float NH){
    // get roughness value (from texture if used)
    float alpha = u_hasRoughTexture ? texture(u_RoughTex, fs_in.texCoords).r : u_material.roughness;
    // remap phong roughness to [0,1]
    alpha = 2 / (alpha * alpha) - 2;
    return (alpha + 2) / 2 * pow(NH, alpha); // dont divide by PI to ignore normalization
}
@endsection