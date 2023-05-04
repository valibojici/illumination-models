#version 330
layout (location = 0) in vec2 in_Position;

out vec3 cubemapCoords;
uniform mat4 u_inverseViewProjMatrix;

void main(){
	// set as far as possible in the direction +Z (towards far plane)
	gl_Position = vec4(in_Position, 0.999f, 1.0f);
	// go from clip space to world space 
	// using inverse(projection) * inverse(view) == inverse(view * projection)
	cubemapCoords = vec3(u_inverseViewProjMatrix * gl_Position);
}