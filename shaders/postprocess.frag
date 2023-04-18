#version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D u_texture;

// what HDR algorithm is used:
// 0 = disabled | 1 = reinhard | 2 = filmic
uniform int u_hdr;

// flag to enable/disable gamma correction
uniform bool  u_gammaCorrect = false; 

uniform float u_exposure = 0.0f;

uniform float u_reinhardWhite = 4;

const float LinearWhite = 11.2; // linear white point value for filmin tonemapping

vec3 filmic_tonemap(vec3 col){
    const float A = 0.22; // shoulder strength
    const float B = 0.30; // linear strength
    const float C = 0.10; // linear angle
    const float D = 0.20; // toe strength
    const float E = 0.01; // toe numerator
    const float F = 0.30; // toe denomimator
   
    return (col * (A * col + C * B) + D * E) / (col * (A * col + B) + D * F) - E / F;
}

vec3 reinhard_tonemap(vec3 col){
    // get luminance
    float L = dot(vec3(0.2126, 0.7152, 0.0722), col);
    // reinhard formula
    float tonemap = L / (1 + L) * (1 + L / (u_reinhardWhite * u_reinhardWhite));
    // scale the color 
    return col * (tonemap / L);
}

void main(){
	vec3 color = texture(u_texture, texCoords).rgb * pow(2, u_exposure);

    if(u_hdr == 1){
        color = reinhard_tonemap(color);
    } else if(u_hdr == 2){
        color = filmic_tonemap(color) / filmic_tonemap(vec3(LinearWhite));
    }

    if(u_gammaCorrect){
        color = pow(color , vec3(1.0 / 2.2));
    }

    outColor = vec4(color, 1.0f);
}