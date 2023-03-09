#version 330 core

out vec4 FragColor;

in VERTEX_TO_FRAGMENT{
    vec3 fragPos;
    vec3 normal;
}fs_in;

uniform vec3 u_color;
uniform vec3 u_viewPos;
uniform vec3 u_lightPos;
uniform float u_diffuseFactor = 0.5f;
uniform float u_specFactor = 1.0f;
uniform float u_shininess = 32;
uniform vec3 u_specColor = vec3(1.0f);

void main()
{
    vec3 normal = normalize(fs_in.normal);
    float ambientFactor = 0.1;
    vec3 ambient = ambientFactor * u_color;

    vec3 lightDir = normalize(u_lightPos - fs_in.fragPos);
    float diff = max(0.0f, dot(lightDir, normal));
    vec3 diffuse = diff * u_color * u_diffuseFactor;

    vec3 viewDir = normalize(u_viewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_shininess);
    vec3 specular = spec * u_specColor * u_specFactor;

    specular *= diff == 0.0f ? 0.0f : 1.0f;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
    // FragColor = vec4(normal / 2.0f + 0.5f, 1.0f);
    //FragColor = vec4(u_color, 1.0f);
}
