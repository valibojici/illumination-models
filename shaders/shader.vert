#version 330 

layout (location = 0) in vec3 in_Position;

uniform mat4 u_modelMatrix = mat4(1.0f);
uniform mat4 u_viewMatrix = mat4(1.0f);
uniform mat4 u_projMatrix = mat4(1.0f);

void main()
{
    gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * vec4(in_Position, 1.0f);
}
