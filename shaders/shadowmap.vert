#version 330 core
layout (location = 0) in vec3 in_Position;

uniform mat4 u_lightSpaceMatrix = mat4(1.0f); // map world space -> light space
uniform mat4 u_modelMatrix = mat4(1.0f); // map to world space

out vec4 fragPos;

void main()
{
    gl_Position = u_lightSpaceMatrix * u_modelMatrix * vec4(in_Position, 1.0);
    fragPos = u_modelMatrix * vec4(in_Position, 1.0f);
}  