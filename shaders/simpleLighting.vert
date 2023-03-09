#version 330 

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_texCoords;
layout (location = 2) in vec3 in_Normal;

uniform mat4 u_modelMatrix = mat4(1.0f);
uniform mat4 u_viewMatrix = mat4(1.0f);
uniform mat4 u_projMatrix = mat4(1.0f);

out VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
}vs_out;

void main()
{
    gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
    vs_out.fragPos = vec3(u_modelMatrix * vec4(in_Position, 1.0f));
    vs_out.normal = vec3(u_modelMatrix * vec4(in_Normal, 0.0f));
}
