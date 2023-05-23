#version 330 core

uniform vec3 u_lightPos; // light position in world space
uniform float u_farPlane; // far plane for light projection matrix

in vec4 fragPos;

void main() { 
	vec3 dist = fragPos.xyz - u_lightPos;
	// use squared distance instead of distance to avoid square root
    // so we have a*a / farplane * farplane instead of sqrt(a*a) / farplane 
	// (map distance to [0,1], divide by farplane because its the maximum distance)
	gl_FragDepth = dot(dist, dist) / pow(u_farPlane, 2);
} 