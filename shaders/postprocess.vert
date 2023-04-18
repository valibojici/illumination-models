#version 330
layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec2 in_TexCoords;

out vec2 texCoords;

void main(){
	gl_Position = vec4(in_Position, 0.0f, 1.0f);
	texCoords = in_TexCoords;
}