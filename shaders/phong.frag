#version 330 core

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

uniform vec3 u_viewPos; // viewer position in world space
uniform int u_numLights; // number of lights
uniform Light u_lights[8]; 

uniform vec3  u_lightCol = vec3(1.0f);  // light color
uniform float u_lightI = 1.0f;          // light intensity (0 to 1)

// Phong material uniforms
uniform float u_kd;                        // diffuse coefficient
uniform vec3  u_diffuseColor = vec3(0.5f); // diffuse color
uniform float u_ks;           // specular coefficient
uniform float u_alpha = 32;   // shininess
uniform float u_ka;           // ambient coefficient
uniform vec3  u_ia;           // ambient intensity/color
uniform vec3  u_emission;     // emission
uniform bool u_modifiedSpecular; // true if specular should NOT be divided by geometry term
// flags for textures
uniform bool u_hasDiffTexture = false;
uniform bool u_hasSpecTexture = false;
uniform bool u_hasNormTexture = false;
uniform sampler2D u_DiffuseTex;
uniform sampler2D u_SpecularTex;
uniform sampler2D u_NormalTex;

uniform bool  u_gammaCorrect = false; // flag to enable/disable gamma correction

// Phong BRDF function
// version == 0 for Phong
// version == 1 for Blinn-Phong
// version == 2 for Phong with normalized specular cos
vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir);

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
    vec3 diffuseCol = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_diffuseColor;
    diffuseCol = u_gammaCorrect ? toLinear(diffuseCol) : diffuseCol;
    vec3 diffuse = u_kd * diffuseCol;

    // calculate specular term
    // get the reflected direction of the light using the (texture) normal
    vec3 reflectDir = reflect(-lightDir, normal);
    // get the angle between the reflection and viewing direction
    float cosPhi = max(0.0f, dot(viewDir, reflectDir));
    // calculate/ get the specular coefficient (from texture)
    float specFactor = u_hasSpecTexture ? texture(u_SpecularTex, fs_in.texCoords).r : u_ks;
    
    float specular = specFactor * pow(cosPhi, u_alpha);
    if(!u_modifiedSpecular){
        specular /= max(0.0001f, geometryTerm);
    }
    
    return diffuse + specular;
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
    vec3 ia = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_ia;
    ia = u_gammaCorrect ? toLinear(ia) : ia;
    return ia * u_ka;
}