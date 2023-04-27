#version 330 
const int MAX_LIGHTS = 5;
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec3 in_Tangent;


uniform mat4 u_modelMatrix = mat4(1.0f);
uniform mat4 u_viewMatrix = mat4(1.0f);
uniform mat4 u_projMatrix = mat4(1.0f);

//TODO use inverse transpose matrix for normal
uniform mat4 u_normalModelMatrix = mat4(1.0f);

out VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
    vec4 fragPosLightSpace[MAX_LIGHTS];
}vs_out;

struct Light{
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
   sampler2D shadowMap;
};

uniform Light u_lights[MAX_LIGHTS]; 

void main()
{
    gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
    //TODO use inverse transpose matrix for normal
    vec3 normal = mat3(u_modelMatrix) * normalize(in_Normal);
    vec3 tangent = mat3(u_modelMatrix) * normalize(in_Tangent);
    vec3 bitangent = cross(normal, tangent);
    // calculate TBN matrix to get the tangent (from texture) from tangent space to world space
    mat3 TBN = mat3(tangent, bitangent, normal);
    vs_out.TBN = TBN;
    vs_out.normal = normal;
    vs_out.fragPos = vec3(u_modelMatrix * vec4(in_Position, 1.0f));
    vs_out.texCoords = in_TexCoords;

    for(int i=0;i<MAX_LIGHTS;++i){
        if(u_lights[i].shadow){
            vs_out.fragPosLightSpace[i] = u_lights[i].lightSpaceMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
        } else {
            vs_out.fragPosLightSpace[i] = vec4(0.0f);
        }
    }
}
