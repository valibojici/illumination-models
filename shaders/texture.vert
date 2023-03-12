#version 330 

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_texCoords;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec3 in_Tangent;

uniform mat4 u_modelMatrix = mat4(1.0f);
uniform mat4 u_viewMatrix = mat4(1.0f);
uniform mat4 u_projMatrix = mat4(1.0f);
uniform mat3 u_transposeInverseModel = mat3(1.0f);

uniform vec3 u_viewPos;
uniform vec3 u_lightPos;

out VERTEX_TO_FRAGMENT{
    vec2 texCoords;
    vec3 viewPos_tangentSpace;
    vec3 lightPos_tangentSpace;
    vec3 fragPos_tangentSpace;
}vs_out;

void main()
{
    gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
    vec3 normal = u_transposeInverseModel * normalize(in_Normal);
    vec3 tangent = u_transposeInverseModel * normalize(in_Tangent);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN_inverse = transpose(mat3(tangent, bitangent, normal));

    vec3 position = vec3(u_modelMatrix * vec4(in_Position, 1.0f));
    vs_out.fragPos_tangentSpace = TBN_inverse * position;
    vs_out.viewPos_tangentSpace = TBN_inverse * u_viewPos;
    vs_out.lightPos_tangentSpace = TBN_inverse * u_lightPos;
    vs_out.texCoords = in_texCoords;
}
