@include "header.partial.frag"

// Material struct
@has "material"

// extra uniforms
@has "extra_uniforms"

/*
    The lighting equation: L_out(x) = Integral( L_in * geometryTerm * BRDF(out,in) )d_in + emission
*/

void main()
{
    vec3 normal = normalize(fs_in.normal);
    // view direction: from fragment to viewer position
    vec3 viewDir = normalize(u_viewPos - fs_in.fragPos);

    // is we use normal mapping get the normal from the texture
    if(u_hasNormTexture){
        normal = texture(u_NormalTex, fs_in.texCoords).rgb * 2.0f - 1.0f;
        normal = normalize(fs_in.TBN * normal);
    }

    // the current contribution of all lights for this fragment
    vec3 result = vec3(0.0f);

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
        
        // calculate geometryTerm using light direction and the normal
        float geometryTerm = max(0.0f, dot(normal, lightDir));

        // gamma correct the light color
        vec3 lightCol = u_gammaCorrect ? toLinear(u_lights[i].color) : u_lights[i].color;

        // get the BRDF result for this light
        vec3 brdfResult = BRDF(geometryTerm, lightDir, normal, viewDir);
        
        result +=  
            u_outputOnlyBRDF * brdfResult + (1 - u_outputOnlyBRDF) * // (branchless toggle for showing the brdf only)
            getShadow(i) *                                           // check if fragment is in shadow
            u_lights[i].intensity * lightCol * geometryTerm *        // light amount at this fragment
            brdfResult *                                             // BRDF
            lightAttenuation(u_lights[i], fs_in.fragPos) *           // attenuation
            spotlightFactor(u_lights[i], lightDir);                  // spotlightFactor
    }
    
    // add ambient light at the end (if not outputting only bdrf)
    result += (1 - u_outputOnlyBRDF) * indirectLighting();

    // add emission
    if(u_hasEmissiveTexture){
        // use emission from texture
        result += texture(u_EmissiveTex, fs_in.texCoords).rgb;
    } else {
        // gamma correct emission color
        result += u_gammaCorrect ? toLinear(u_emission) : u_emission;
    }
    
    // handle opacity texture
    if(u_hasOpacityTexture){
        // get opacity from red channel
        float opacity = texture(u_OpacityTex, fs_in.texCoords).r;
        FragColor = vec4(result, opacity);
    } else {
        FragColor = vec4(result, 1.0f);
    }
}

@has "BRDF_implementation"

@include "footer.partial.frag"