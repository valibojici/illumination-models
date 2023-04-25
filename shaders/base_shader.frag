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
    vec3 normal = fs_in.normal;
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
        if(u_lights[i].position.w == 0){
            lightDir = normalize(u_lights[i].position.xyz); // the direction is the light "position"
        }
        
        // calculate geometryTerm using light direction and the actual surface normal
        float geometryTerm = max(0.0f, dot(fs_in.normal, lightDir));

        // gamma correct the light color
        vec3 lightCol = u_gammaCorrect ? toLinear(u_lights[i].color) : u_lights[i].color;

        result += getShadow(i) *
            u_lights[i].intensity * lightCol * geometryTerm *       // light amount at this fragment
            BRDF(geometryTerm, lightDir, normal, viewDir) *         // BRDF
            lightAttenuation(u_lights[i], fs_in.fragPos) *          // attenuation
            spotlightFactor(u_lights[i], lightDir);                 // spotlightFactor
    }
    
    // add ambient light at the end
    result += indirectLighting();

    // add emission
    result += u_gammaCorrect ? toLinear(u_emission) : u_emission;
    
    // gamma correct the output
    FragColor = vec4(result, 1.0f);
}

@has "BRDF_implementation"

@include "footer.partial.frag"