#version 330 
const int MAX_LIGHTS = 5;
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec3 in_Tangent;


uniform mat4 u_modelMatrix = mat4(1.0f);
uniform mat4 u_viewMatrix = mat4(1.0f);
uniform mat4 u_projMatrix = mat4(1.0f);

out VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;       // matrix to transform normal from tangent space to world space
    vec4 fragPosLightSpace[MAX_LIGHTS]; // the fragment position in lightspace, for every light (for shadows)
}vs_out;

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

uniform Light u_lights[MAX_LIGHTS]; 
// scale texture coords by these
uniform float u_textureScaleX = 1.0f;
uniform float u_textureScaleY = 1.0f;

void main()
{
    gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
    //TODO use inverse transpose matrix for normal
    vec3 normal = mat3(u_modelMatrix) * normalize(in_Normal);
    vec3 tangent = mat3(u_modelMatrix) * normalize(in_Tangent);
    // calculate bitangent
    vec3 bitangent = cross(normal, tangent);
    // calculate TBN matrix to get the tangent (from texture) from tangent space to world space
    mat3 TBN = mat3(tangent, bitangent, normal);
    vs_out.TBN = TBN;
    vs_out.normal = normal;
    vs_out.fragPos = vec3(u_modelMatrix * vec4(in_Position, 1.0f)); // fragment position in world space
    vs_out.texCoords = vec2(u_textureScaleX, u_textureScaleY) * in_TexCoords;

    // calculate fragment position in lightspace for every light
    for(int i=0;i<MAX_LIGHTS;++i){
        vs_out.fragPosLightSpace[i] = u_lights[i].lightSpaceMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
    }
}
