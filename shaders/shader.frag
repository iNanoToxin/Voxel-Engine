#version 430 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


in vec3 o_Normal;
in vec3 o_FragPos;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;

void main()
{
    vec3 norm = normalize(o_Normal);
    vec3 lightDir = normalize(u_Light.position - o_FragPos);
    vec3 viewDir = normalize(u_ViewPos - o_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    vec3 ambient = u_Light.ambient * u_Material.ambient;
    vec3 diffuse = u_Light.diffuse * u_Material.diffuse * diff;
    vec3 specular = u_Light.specular * u_Material.specular * spec;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
