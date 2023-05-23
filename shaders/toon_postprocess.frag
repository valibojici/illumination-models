#version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D u_colorTex;
uniform sampler2D u_distNormalTex;

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

uniform float u_textureWidth = 1280;
uniform float u_textureHeight = 720;

void main(){
	vec3 color = texture(u_colorTex, texCoords).rgb; // get fragment color
    float dist = texture(u_distNormalTex, texCoords).a; // get fragment distance (scaled by farplane)

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

    // use Sobel operator to detect edges: https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=676679e17b9033b8a1eb1c2618cb5bd9e3c4504c

    // vertical sobel kernel
     const float kernelV[9] = float[](
        1, 2, 1,
        0,  0, 0,
        -1, -2, -1
    );

    // horizontal sobel kernel
    const float kernelH[9] = float[](
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
        Gx += normal * kernelH[i]; // horizontal
        Gy += normal * kernelV[i]; // vertical
    }
    vec3 G = abs(Gx) + abs(Gy); // combine results
    
    // assume if one of the channels is >= ~0.6 then there is an edge
    float edge = 0;
    if(G.x > 0.6 || G.y > 0.6 || G.z > 0.6){
        edge = 0;
    } else {
        edge = 1;
    }

    outColor = vec4(color * edge, 1.0f);
}