#version 330

in vec3 cubemapCoords;
out vec4 FragColor;

uniform samplerCube u_texture;

void main(){
	FragColor = vec4(texture(u_texture, cubemapCoords).rgb, 1.0f);
}