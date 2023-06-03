// map color from srgb to linear
vec3 toLinear(vec3 color){
    return pow(color, vec3(2.2f));
}

float lightAttenuation(Light light, vec3 fragPos){
    // if light is directional => no attenuation
    if(light.type == 0){
        return 1;
    }
    // get the distance from the fragment to the light
    float dist = max(0.00001f, distance(light.position.xyz, fragPos));
    // use the parameters from the light for: constant, linear, quadratic terms
    return 1.0f / dot(light.attenuation, vec3(1.0f, dist, dist*dist));
}

float spotlightFactor(Light light, vec3 lightDir){
    // if light is not spotlight => ignore spotlight factor
    if(light.type != 1){
        return 1;
    }
    // get the spotlight direction: from the light to the target
	vec3 spotDir = normalize(light.target - light.position.xyz);
    // get the cos of the angle between the spotlight direction (light to target) and light direction (light to fragment, -lightDir)
	float cosSpotDir_LightDir = dot(spotDir, -lightDir);
    // formula from RealTime Rendering p. 115 (used in Frostbite engine)
	return pow(clamp((cosSpotDir_LightDir - light.outerCutOff) / (light.cutOff - light.outerCutOff) , 0.0f, 1.0f), 2);
}

// approximate indirect lighting with constant
vec3 indirectLighting(){
    // get ambient color from diffuse color 
    vec3 ia = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_material.ia;
    ia = u_gammaCorrect ? toLinear(ia) : ia;
    return ia * u_material.ka;
}

float getShadow(int index){
    // if light has no shadows => skip
    if(u_lights[index].shadow == false) return 1.0f;

    // get the fragment depth from texture
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
        // if not pointlight, get lightspace coords in clip space (divide by W term)
        vec3 fragPosLightSpace = fs_in.fragPosLightSpace[index].xyz / fs_in.fragPosLightSpace[index].w;
        vec3 fragPosLightTexCoords = fragPosLightSpace * 0.5 + 0.5; // map to [0,1]
        switch(index){
            case 0: shadowMapDepth = texture(u_lights[0].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 1: shadowMapDepth = texture(u_lights[1].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 2: shadowMapDepth = texture(u_lights[2].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 3: shadowMapDepth = texture(u_lights[3].shadowMap, fragPosLightTexCoords.xy).r; break;
            case 4: shadowMapDepth = texture(u_lights[4].shadowMap, fragPosLightTexCoords.xy).r; break;
        }
    }
    // get the current coords difference between fragment and light
    vec3 coordsDifference = fs_in.fragPos.xyz - u_lights[index].position.xyz;

    // use squared distance instead of distance to avoid square root
    // so we have a*a / farplane * farplane instead of sqrt(a*a) / farplane
    // divide distance by farplane to get [0,1] values and compare
    float currentDepth = dot(coordsDifference, coordsDifference) / pow(u_lights[index].farPlane, 2);
    float bias = 0.001f;
    // add some bias to the depth from the shadowmap texture
    bool in_shadow = currentDepth > shadowMapDepth + bias ? true : false;
    
    if(currentDepth > 1) { // if outside the far plane => no shadow
        in_shadow = false;
    }
   
    return in_shadow == true ? 0.0f : 1.0f;
}