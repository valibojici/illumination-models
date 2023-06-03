#version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D u_texture; // texture to display

uniform bool u_hdr; // flag enable/disable HDR

uniform bool  u_gammaCorrect = false; // flag to enable/disable gamma correction

// value for White in reinhard mapping (smallest luminance mapped to pure white)
uniform float u_reinhardWhite = 4; 

vec3 reinhard_tonemap(vec3 col){
    // get luminance https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-6-201506-I!!PDF-E.pdf p. 4
    float luminance = dot(vec3(0.2126, 0.7152, 0.0722), col);
    // Reinhard formula from "Photographic Tone Reproduction for Digital Images"
    float scaled_luminance = luminance / (1 + luminance) * (1 + luminance / (u_reinhardWhite * u_reinhardWhite));
    // scale the color
    return col * (scaled_luminance / luminance);
}

void main(){
    // get color to output
	vec3 color = texture(u_texture, texCoords).rgb;

    if(u_hdr){
        color = reinhard_tonemap(color);
    }

    if(u_gammaCorrect){
        color = pow(color , vec3(1.0 / 2.2));
    }

    outColor = vec4(color, 1.0f);
}