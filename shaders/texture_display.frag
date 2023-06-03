#version 330

// simple shader to display a texture on screen

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D u_texture;

void main(){
	vec3 color = texture(u_texture, texCoords).rgb;
    outColor = vec4(color, 1.0f);
}