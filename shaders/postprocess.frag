#version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D u_texture;

// flag if HDR is used:
uniform bool u_hdr;

// flag to enable/disable gamma correction
uniform bool  u_gammaCorrect = false; 

uniform float u_reinhardWhite = 4;

vec3 reinhard_tonemap(vec3 col){
    // get luminance
    float L = dot(vec3(0.2126, 0.7152, 0.0722), col);
    // reinhard formula
    float tonemap = L / (1 + L) * (1 + L / (u_reinhardWhite * u_reinhardWhite));
    // scale the color 
    return col * (tonemap / L);
}

void main(){
	vec3 color = texture(u_texture, texCoords).rgb;

    if(u_hdr){
        color = reinhard_tonemap(color);
    }

    if(u_gammaCorrect){
        color = pow(color , vec3(1.0 / 2.2));
    }

    outColor = vec4(color, 1.0f);
}