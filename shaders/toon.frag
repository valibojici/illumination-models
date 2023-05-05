#version 330 core
const int MAX_LIGHTS = 5;
const float PI = 3.14159265359;
const float SQRT_PI = 1.77245385091;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 distance_normal;

in VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    mat3 TBN;
    vec4 fragPosLightSpace[MAX_LIGHTS];
}fs_in;

struct Light{
   int type; // 0 == directional | 1 == spotlight | 2 == pointlight
   vec4 position;       // w == 0 for directional
   float intensity;     // 0 to 1
   vec3 color;
   bool enabled;
   vec3 attenuation;    // constant, linear, quadratic
   vec3 target;         // for spotlight
   float cutOff;        // cos value ---> 1 if not spotlight
   float outerCutOff;   // cos value
   bool shadow;         // if casting shadow
   mat4 lightSpaceMatrix;
   float farPlane;
   sampler2D shadowMap;
   samplerCube shadowMapCube;
};

uniform vec3 u_viewPos;                 // viewer position in world space
uniform int u_numLights;                // number of lights
uniform Light u_lights[MAX_LIGHTS]; 

uniform bool  u_gammaCorrect = false; // flag to enable/disable gamma correction

uniform vec3 u_emission;         // emission

vec3 BRDF(float geometryTerm, vec3 lightDir, vec3 normal, vec3 viewDir);

// helper function to convert to linear from SRGB (raise to 2.2)
vec3 toLinear(vec3 color);

// helper function to calculate spotlight factor (the cone) for a light
float spotlightFactor(Light light, vec3 lightDir);

// helper function to calculate shadow factor (0 = in shadow)
float getShadow(int index);

struct Material{
   vec3 diffuseColor;     // "diffuse" color
   vec3 ambientColor;
};
uniform Material u_material;

void main()
{
    vec3 ambientColor = u_gammaCorrect ? toLinear(u_material.ambientColor) : u_material.ambientColor;
    vec3 diffuseColor = u_gammaCorrect ? toLinear(u_material.diffuseColor) : u_material.diffuseColor;

    vec3 normal = normalize(fs_in.normal);
    // view direction: from fragment to viewer position
    vec3 viewDir = normalize(u_viewPos - fs_in.fragPos);

    // count how many lights fully lit/partially lit fragment
    int isFullyLit = 0;
    int isPartiallyLit = 0;
    // go through each light and calculate the contribution
    for(int i=0;i<u_numLights;++i){
        // if light is disabled => skip
        if(u_lights[i].enabled == false){
            continue;
        }
        // light direction: from fragment to light position
        vec3 lightDir = normalize(u_lights[i].position.xyz - fs_in.fragPos);    
        
        // check if light is directional
        if(u_lights[i].type == 0){
            lightDir = normalize(u_lights[i].position.xyz); // the direction is the light "position"
        }

        // check if light is behind
        if(dot(normalize(fs_in.normal), lightDir) < 0.0f) continue;
        
        // calculate geometryTerm using light direction and the actual surface normal
        float geometryTerm = max(0.0f, dot(normal, lightDir));


        // gamma correct the light color
        vec3 lightCol = u_gammaCorrect ? toLinear(u_lights[i].color) : u_lights[i].color;

        vec3 currentResult = getShadow(i) *
            u_lights[i].intensity * lightCol * geometryTerm *       // light amount at this fragment
            diffuseColor *
            spotlightFactor(u_lights[i], lightDir);                 // spotlightFactor
        
        // check if fragment is lit at all
        bool isLit = dot(vec3(1.0f), currentResult) > 0;
        if(!isLit) continue;

        // check if it is partially/fully lit
        if(geometryTerm < 0.5f) isPartiallyLit++;
        else if(geometryTerm > 0.5f) isFullyLit++;

        // accumulate light color and intensity
        diffuseColor *= u_lights[i].intensity * lightCol;
    }

    // if not lit at all output ambient color 
    if(isFullyLit + isPartiallyLit == 0){
        fragColor = vec4(ambientColor, 1.0f);    
    } else {
        // else check if it is more fully lit than partial lit
        float diffuseFactor = isPartiallyLit > isFullyLit ? 0.5f : 1.0f;
        // partially lit => half diffuse color
        fragColor = vec4(ambientColor + diffuseColor * diffuseFactor, 1.0f);    
    }
    // output normal mapped to [0,1] and distance to fragment to second color attachment
    distance_normal = vec4((normal + 1) / 2, length(u_viewPos - fs_in.fragPos) / 50);
}


vec3 toLinear(vec3 color){
    return pow(color, vec3(2.2f));
}

float spotlightFactor(Light light, vec3 lightDir){
    if(light.type != 1){
        return 1;
    }
    // get the spotlight direction: from the light to the target
	vec3 spotDir = normalize(light.target - light.position.xyz);
    // get the cos of the angle between the spotlight direction (light to target) and light direction (light to fragment)
	float cosSpotDir_LightDir = dot(spotDir, -lightDir);
    // TODO research more
	return clamp((cosSpotDir_LightDir - light.outerCutOff) / (light.cutOff - light.outerCutOff) , 0.0f, 1.0f);
}

float getShadow(int index){
    if(u_lights[index].shadow == false) return 1.0f;

    float shadowMapDepth;

    // if light is pointlight sample cubemap
    if(u_lights[index].type == 2){
        vec3 lightToFrag = fs_in.fragPos - u_lights[index].position.xyz;
        switch(index){
            case 0: shadowMapDepth = texture(u_lights[0].shadowMapCube, lightToFrag).r; break;
            case 1: shadowMapDepth = texture(u_lights[1].shadowMapCube, lightToFrag).r; break;
            case 2: shadowMapDepth = texture(u_lights[2].shadowMapCube, lightToFrag).r; break;
            case 3: shadowMapDepth = texture(u_lights[3].shadowMapCube, lightToFrag).r; break;
            case 4: shadowMapDepth = texture(u_lights[4].shadowMapCube, lightToFrag).r; break;
        }
    }
    else {
        vec3 fragPosLightSpace = fs_in.fragPosLightSpace[index].xyz / fs_in.fragPosLightSpace[index].w;
        vec3 fragPosLightTexCoords = fragPosLightSpace * 0.5 + 0.5;
        switch(index){
            case 0: shadowMapDepth = texture(u_lights[0].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 1: shadowMapDepth = texture(u_lights[1].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 2: shadowMapDepth = texture(u_lights[2].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 3: shadowMapDepth = texture(u_lights[3].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 4: shadowMapDepth = texture(u_lights[4].shadowMap, fragPosLightTexCoords.xy).r; break;
        }
    }
    vec3 currentDist = fs_in.fragPos.xyz - u_lights[index].position.xyz;

    // use squared distance instead of distance to avoid square root
    // so we have a*a / farplane * farplane instead of sqrt(a*a) / farplane
    float currentDepth = dot(currentDist, currentDist) / pow(u_lights[index].farPlane, 2);
    float bias = 0.001f;
    // add some bias to the depth from the shadowmap texture
    bool in_shadow = currentDepth > shadowMapDepth + bias ? true : false;
    
    if(currentDepth > 1) { // if outside the far plane => no shadow
        in_shadow = false;
    }
   
    return in_shadow == true ? 0.0f : 1.0f;
}