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
   int type; // 0 == directional | 1 == spotlight | 2 == pointlight
   vec4 position;       // w == 0 for directional
   float intensity;     // 0 to 1
   vec3 color;
   bool enabled;
   vec3 attenuation;    // constant, linear, quadratic
   vec3 target;         // for spotlight
   float cutOff;        // cos value ---> 1 if not spotlight
   float outerCutOff;   // cos value
   bool shadow;         // if casting shadow
   mat4 lightSpaceMatrix;
   float farPlane;
   sampler2D shadowMap;
   samplerCube shadowMapCube;
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

uniform vec3 u_emission;         // emission

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