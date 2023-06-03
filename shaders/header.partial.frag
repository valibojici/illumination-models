#version 330 core
const int MAX_LIGHTS = 5;
const float PI = 3.14159265359;
const float SQRT_PI = 1.77245385091;

out vec4 FragColor;

in VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
    vec4 fragPosLightSpace[MAX_LIGHTS];
}fs_in;

struct Light{
   int type;        // 0 == directional | 1 == spotlight | 2 == pointlight
   vec4 position;   // light position in world space    
   float intensity; // 0 = no light
   vec3 color;
   bool enabled;        // flag if light is active
   vec3 attenuation;    // distance attenuation: constant, linear, quadratic
   vec3 target;         // spotlight target in world space
   float cutOff;        // cos value of spotlight inner cut off angle
   float outerCutOff;   // cos value of spotlight outer cut off angle, inner < outer
   bool shadow;         // enable/disable using shadows
   mat4 lightSpaceMatrix; // used to transform fragment from world space to light space
   float farPlane;        // used for shadows
   sampler2D shadowMap;   // shadow map texture for directional / spotlight
   samplerCube shadowMapCube; // shadow map texture for point light
};

uniform vec3 u_viewPos;                 // viewer position in world space
uniform int u_numLights;                // number of lights
uniform Light u_lights[MAX_LIGHTS]; 

uniform bool  u_gammaCorrect = false; // flag to enable/disable gamma correction

// flags for textures
uniform bool u_hasDiffTexture = false;
uniform bool u_hasSpecTexture = false;
uniform bool u_hasRoughTexture = false;
uniform bool u_hasNormTexture = false;
uniform bool u_hasMetallicTexture = false;
uniform bool u_hasEmissiveTexture = false;
uniform bool u_hasOpacityTexture = false;
uniform sampler2D u_DiffuseTex;
uniform sampler2D u_SpecularTex;
uniform sampler2D u_NormalTex;
uniform sampler2D u_RoughTex;
uniform sampler2D u_MetallicTex;
uniform sampler2D u_EmissiveTex;
uniform sampler2D u_OpacityTex;

uniform vec3 u_emission; // emission color

uniform int u_outputOnlyBRDF = 0; // flag to output only the BRDF

vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir);

// helper function to convert to linear from SRGB (raise to 2.2)
vec3 toLinear(vec3 color);

// helper function to calculate attenuation factor for a light
float lightAttenuation(Light light, vec3 fragPos);

// helper function to calculate spotlight factor (the cone) for a light
float spotlightFactor(Light light, vec3 lightDir);

// helper function to calculate indirect lighting (ambient)
vec3 indirectLighting();

// helper function to calculate shadow factor (0 = in shadow)
float getShadow(int index);