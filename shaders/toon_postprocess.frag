#version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D u_colorTex;
uniform sampler2D u_distNormalTex;

// if HDR is used
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

uniform float u_textureWidth = 1280;
uniform float u_textureHeight = 720;

void main(){
	vec3 color = texture(u_colorTex, texCoords).rgb;
    float dist = texture(u_distNormalTex, texCoords).a;

    if(u_hdr){
        color = reinhard_tonemap(color);
    }

    if(u_gammaCorrect){
        color = pow(color , vec3(1.0 / 2.2));
    }

    // make texture sampling offsets smaller with distance
    float xOffset = 3.0f / (u_textureWidth + 3.0f * dist * u_textureWidth); 
    float yOffset = 3.0f / (u_textureHeight + 3.0f * dist * u_textureHeight);

    float[9] xOffsets = float[](-xOffset, 0, xOffset, -xOffset, 0, xOffset, -xOffset, 0, xOffset);
    float[9] yOffsets = float[](yOffset, yOffset, yOffset, 0, 0, 0, -yOffset, -yOffset, -yOffset);

    // vertical sobel kernel
     float kernelV[9] = float[](
        1, 2, 1,
        0,  0, 0,
        -1, -2, -1
    );

    // horizontal sobel kernel
     float kernelH[9] = float[](
        -1, 0, 1,
        -2,  0, 2,
        -1, 0, 1
    );

    vec3 Gx = vec3(0.0);
    vec3 Gy = vec3(0.0);
    // apply sobel convolution
    for(int i = 0; i < 9; i++)
    {
        vec3 normal = vec3(texture(u_distNormalTex, texCoords + vec2(xOffsets[i], yOffsets[i])).rgb);
        Gx += normal * kernelH[i];
        Gy += normal * kernelV[i];
    }
    vec3 G = abs(Gx) + abs(Gy);
    
    // if one of the channels is >= ~0.6 then there is an edge
    float edge = 0;
    if(G.x > 0.6 || G.y > 0.6 || G.z > 0.6){
        edge = 0;
    } else {
        edge = 1;
    }

    outColor = vec4(color * edge, 1.0f);
}