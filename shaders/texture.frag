#version 330 core

out vec4 FragColor;

in VERTEX_TO_FRAGMENT{
    vec2 texCoords;
    vec3 viewPos_tangentSpace;
    vec3 lightPos_tangentSpace;
    vec3 fragPos_tangentSpace;
}fs_in;

uniform vec3 u_color;
uniform vec3 u_emission = vec3(0.0f);
uniform float u_diffuseFactor = 0.5f;
uniform float u_specFactor = 1.0f;
uniform float u_shininess = 32;
uniform vec3 u_specColor = vec3(1.0f);

uniform sampler2D u_DiffuseTex;
uniform sampler2D u_SpecularTex;
uniform sampler2D u_NormalTex;

uniform bool u_normalMapping = false;

void main()
{
    vec3 normal = vec3(0.0f, 0.0f, 1.0f);
    if(u_normalMapping){
        normal = texture(u_NormalTex, fs_in.texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0); 
    }
    
    float ambientFactor = 0.1;
    vec3 ambient = ambientFactor * texture(u_DiffuseTex, fs_in.texCoords).rgb;

    vec3 lightDir = normalize(fs_in.lightPos_tangentSpace - fs_in.fragPos_tangentSpace);
    float geometryTerm = max(0.0f, lightDir.z); // dot(lightDir, {0,0,1})
    float diff = max(0.0f, dot(lightDir, normal));
    vec3 diffuse = diff * texture(u_DiffuseTex, fs_in.texCoords).rgb * u_diffuseFactor;

    vec3 viewDir = normalize(fs_in.viewPos_tangentSpace- fs_in.fragPos_tangentSpace);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_shininess);
    vec3 specular = spec * texture(u_SpecularTex, fs_in.texCoords).rgb * u_specFactor;


    FragColor = vec4(ambient + (diffuse + specular)*geometryTerm + u_emission, 1.0f);
}
