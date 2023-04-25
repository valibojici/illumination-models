vec3 toLinear(vec3 color){
    return pow(color, vec3(2.2f));
}

float lightAttenuation(Light light, vec3 fragPos){
    if(light.position.w == 0){
        return 1;
    }
    // get the distance from the fragment to the light
    float dist = max(0.00001f, distance(light.position.xyz, fragPos));
    return 1.0f / dot(light.attenuation, vec3(1.0f, dist, dist*dist));
}

float spotlightFactor(Light light, vec3 lightDir){
    if(light.position.w == 0 || light.cutOff == 1){
        return 1;
    }
    // get the spotlight direction: from the light to the target
	vec3 spotDir = normalize(light.target - light.position.xyz);
    // get the cos of the angle between the spotlight direction (light to target) and light direction (light to fragment)
	float cosSpotDir_LightDir = dot(spotDir, -lightDir);
    // TODO research more
	return clamp((cosSpotDir_LightDir - light.outerCutOff) / (light.cutOff - light.outerCutOff) , 0.0f, 1.0f);
}

vec3 indirectLighting(){
    vec3 ia = u_hasDiffTexture ? texture(u_DiffuseTex, fs_in.texCoords).rgb : u_material.ia;
    ia = u_gammaCorrect ? toLinear(ia) : ia;
    return ia * u_material.ka;
}

float getShadow(int index){
    if(u_lights[index].shadow == false) return 1.0f;

    vec3 fragPosLightSpace = fs_in.fragPosLightSpace[index].xyz / fs_in.fragPosLightSpace[index].w;
    vec3 fragPosLightTexCoords = fragPosLightSpace * 0.5 + 0.5;

    float shadowMapDepth;
    switch(index){
        case 0: shadowMapDepth = texture(u_shadowTex[0], fragPosLightTexCoords.xy).r; break;
        case 1: shadowMapDepth = texture(u_shadowTex[1], fragPosLightTexCoords.xy).r; break;
        case 2: shadowMapDepth = texture(u_shadowTex[2], fragPosLightTexCoords.xy).r; break;
        case 3: shadowMapDepth = texture(u_shadowTex[3], fragPosLightTexCoords.xy).r; break;
        case 4: shadowMapDepth = texture(u_shadowTex[4], fragPosLightTexCoords.xy).r; break;
    }
    
    bool in_shadow = fragPosLightSpace.z > shadowMapDepth + 0.005f ? true : false;
    
    if(fragPosLightSpace.z > 1.0f) {
        in_shadow = false;
    }
    //return shadowMapDepth;
    return in_shadow == true ? 0.0f : 1.0f;

}