#version 330 core
const float PI = 3.14159265359;
const float SQRT_PI = 1.77245385091;

out vec4 FragColor;

in VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
}fs_in;

struct Light{
   vec4 position; // w == 0 for directional
   float intensity; // 0 to 1
   vec3 color;
   bool enabled;
   vec3 attenuation; // constant, linear, quadratic
   
   vec3 target; // for spotlight
   float cutOff; // cos value ---> 1 if not spotlight
   float outerCutOff; // cos value
};

struct Material{
   vec3 albedo;     // "diffuse" color
   float roughness; // 0 = mirror | 1 = very rought
   vec3 f0;         // reflectance function at normal incidence (for fresnel)
   float ratio;     // => mix(diffuse, specular, ratio)
   vec3  ia;        // ambient intensity/color
   float ka;        // ambient coefficient
};

uniform vec3 u_viewPos; // viewer position in world space
uniform int u_numLights; // number of lights
uniform Light u_lights[8]; 

uniform Material u_material;
uniform int u_Gindex = 0;
uniform int u_Dindex = 0;

uniform vec3  u_emission;     // emission
// flags for textures
uniform bool u_hasDiffTexture = false;
uniform bool u_hasSpecTexture = false;
uniform bool u_hasNormTexture = false;
uniform sampler2D u_DiffuseTex;
uniform sampler2D u_SpecularTex;
uniform sampler2D u_NormalTex;

uniform bool  u_gammaCorrect = false; // flag to enable/disable gamma correction

vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir);

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

// helper function to convert to SRGB from linear (raise to 1/2.2)
vec3 toSRGB(vec3 color);

// helper function to convert to linear from SRGB (raise to 2.2)
vec3 toLinear(vec3 color);

// helper function to calculate attenuation factor for a light
float lightAttenuation(Light light, vec3 fragPos);

// helper function to calculate spotlight factor (the cone) for a light
float spotlightFactor(Light light, vec3 lightDir);

// helper function to calculate indirect lighting (ambient)
vec3 indirectLighting();

/*
    The lighting equation: L_out(x) = Integral( L_in * geometryTerm * BRDF(out,in) )d_in + emission
*/

void main()
{
    vec3 normal = fs_in.normal;
    // view direction: from fragment to viewer position
    vec3 viewDir = normalize(u_viewPos - fs_in.fragPos);

    // is we use normal mapping get the normal from the texture
    if(u_hasNormTexture){
        normal = texture(u_NormalTex, fs_in.texCoords).rgb * 2.0f - 1.0f;
        normal = normalize(fs_in.TBN * normal);
    }

    // the current contribution of all lights for this fragment
    vec3 result = vec3(0.0f);

    // go through each light and calculate the contribution
    for(int i=0;i<u_numLights;++i){
        // if light is disabled => skip
        if(u_lights[i].enabled == false){
            continue;
        }
        // light direction: from fragment to light position
        vec3 lightDir = normalize(u_lights[i].position.xyz - fs_in.fragPos);    
        
        // check if light is directional
        if(u_lights[i].position.w == 0){
            lightDir = normalize(u_lights[i].position.xyz); // the direction is the light "position"
        }
        
        // calculate geometryTerm using light direction and the actual surface normal
        float geometryTerm = max(0.0f, dot(fs_in.normal, lightDir));

        // gamma correct the light color
        vec3 lightCol = u_gammaCorrect ? toLinear(u_lights[i].color) : u_lights[i].color;
        
        result += 
            u_lights[i].intensity * lightCol * geometryTerm *       // light amount at this fragment
            BRDF(geometryTerm, lightDir, normal, viewDir) *         // BRDF
            lightAttenuation(u_lights[i], fs_in.fragPos) *          // attenuation
            spotlightFactor(u_lights[i], lightDir);                 // spotlightFactor
    }
    
    // add ambient light at the end
    result += indirectLighting();

    // add emission
    result += u_gammaCorrect ? toLinear(u_emission) : u_emission;
    
    // gamma correct the output
    FragColor = vec4(u_gammaCorrect ? toSRGB(result) : result, 1.0f);
}

vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir){
    // calculate diffuse term

    // get the diffuse color from texture and do gamma correction
    vec3 diffuse = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_material.albedo;
    diffuse = u_gammaCorrect ? toLinear(diffuse) : diffuse;

    // calculate specular term
    // get the halfway direction vector between light direction and view direction
    vec3 halfway = normalize(lightDir + viewDir);

    // calculate/ get the specular coefficient (from texture)
    float roughness = u_material.roughness;


    float NH = dot(normal, halfway);
    float NL = dot(normal, lightDir);
    float VH = dot(viewDir, halfway); // u
    float NV = dot(normal, viewDir);
    
    vec3 fresnel = F_Schlick(u_material.f0, VH);
    float slope_distribution;
    float geometrical_attenuation;

    switch(u_Dindex){
        case 0: slope_distribution = D_Beckmann(NH); break;
        case 1: slope_distribution = D_GGX(NH); break;
        case 2: slope_distribution = D_Phong(NH);
    }
   
    switch(u_Gindex){
        case 0: geometrical_attenuation = G_Cook(VH, NH, NV, NL); break;
        case 1: geometrical_attenuation = G2_U_Beckmann(NV, NL); break;
        case 2: geometrical_attenuation = G2_U_GGX(NV, NL); break;
        case 3: geometrical_attenuation = G2_Beckmann(NV, NL); break;
        case 4: geometrical_attenuation = G2_GGX(NV, NL); break;
    }

    vec3 specular =  (fresnel * slope_distribution * geometrical_attenuation) / 
                    (4 * max(0.0001f, NL) * max(0.0001f, NV));
    
    return mix(diffuse, PI * specular, u_material.ratio);
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
    float alpha = u_material.roughness;
    
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
    float alpha = u_material.roughness;
    float a_V = NV / (alpha * sqrt(1-NV*NV));
    float lambda_V = a_V >= 1.6f ? 0.0f : (1 - 1.259*a_V + 0.396*a_V*a_V) / (3.535*a_V + 2.181 * a_V * a_V);

    float a_L = NL / (alpha * sqrt(1-NL*NL));
    float lambda_L = a_L >= 1.6f ? 0.0f : (1 - 1.259*a_L + 0.396*a_L*a_L) / (3.535*a_L + 2.181 * a_L * a_L);

    return 1 / (1 + lambda_L + lambda_V);
}

// GGX
float G2_U_GGX(float NV, float NL){
    float alpha = u_material.roughness;
    float sq_alpha = alpha * alpha;
    float g_V = 2 * NV / (NV + sqrt(NV*NV + sq_alpha * (1 - NV * NV)));
    float g_L = 2 * NL / (NL + sqrt(NL*NL + sq_alpha * (1 - NL * NL)));
    return g_V * g_L;
}

float G2_GGX(float NV, float NL){
    float alpha = u_material.roughness;
    float sq_alpha = alpha * alpha;

    return 2 * NL * NV / (NL * sqrt(sq_alpha + NV * (NV - alpha * NV)) + NV * sqrt(sq_alpha + NL * (NL - alpha * NL)));
}

// slope distribution function
// beckmann version, used in cook-torrance paper
float D_Beckmann(float NH){
    float a = exp((NH * NH - 1) / (NH * NH * u_material.roughness * u_material.roughness));
    float b = pow(u_material.roughness, 2) * pow(NH, 4);
    return a / (PI * b);
}
// beckmann with schlick approximation
float D_GGX(float NH){
    float alpha = u_material.roughness * u_material.roughness;
    return alpha * alpha / (PI * pow((pow(NH, 4) * (alpha * alpha - 1) + 1), 2));
}

// phong distribution
float D_Phong(float NH){
    float alpha = 2 / (u_material.roughness * u_material.roughness) - 2;
    return (alpha + 2) / (2 * PI) * pow(NH, alpha);
}


vec3 toSRGB(vec3 color){
    return pow(color, vec3(1 / 2.2f));
}

vec3 toLinear(vec3 color){
    return pow(color, vec3(2.2f));
}

float lightAttenuation(Light light, vec3 fragPos){
    if(light.position.w == 0){
        return 1;
    }
    // get the distance from the fragment to the light
    float dist = max(0.00001f, distance(light.position.xyz, fragPos));
    return 1.0f / dot(light.attenuation, vec3(1.0f, dist, dist*dist));
}

float spotlightFactor(Light light, vec3 lightDir){
    if(light.position.w == 0 || light.cutOff == 1){
        return 1;
    }
    // get the spotlight direction: from the light to the target
	vec3 spotDir = normalize(light.target - light.position.xyz);
    // get the cos of the angle between the spotlight direction (light to target) and light direction (light to fragment)
	float cosSpotDir_LightDir = dot(spotDir, -lightDir);
    // TODO research more
	return clamp((cosSpotDir_LightDir - light.outerCutOff) / (light.cutOff - light.outerCutOff) , 0.0f, 1.0f);
}

vec3 indirectLighting(){
    vec3 ia = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_material.ia;
    ia = u_gammaCorrect ? toLinear(ia) : ia;
    return ia * u_material.ka;
}