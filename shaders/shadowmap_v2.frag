#version 330 core

uniform vec3 u_lightPos;
uniform float u_farPlane;

in vec4 fragPos;

void main() { 
	vec3 dist = fragPos.xyz - u_lightPos;
	// use squared distance instead of distance to avoid square root
    // so we have a*a / farplane * farplane instead of sqrt(a*a) / farplane
	gl_FragDepth = dot(dist, dist) / pow(u_farPlane, 2);
} 