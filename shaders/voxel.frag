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
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};


in vec3 o_Normal;
in vec3 o_FragPos;
in vec3 o_TexCoord;
flat in ivec3 o_Scale;
flat in int o_TexIndex;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;
uniform samplerCubeArray u_Texture;

float scale(float coord, float scale)
{
    // transform range from [-0.5, 0.5] to [0.0, 1.0]
    coord = coord + 0.5;

    // edge cases to remove artifacts (floating point precision error)
    if (coord <= 1e-5)
    {
        return -0.5;
    }
    else if (coord >= 1.0 - 1e-5)
    {
        return 0.5;
    }
    // wrap texture and transform range to [-0.5, 0.5]
    return mod(coord * scale, 1.0) - 0.5;
}

void main()
{
    vec3 norm = normalize(o_Normal);
    vec3 lightDir = normalize(u_Light.position - o_FragPos);
    vec3 viewDir = normalize(u_ViewPos - o_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    diff = max(diff, 0.5);

    vec3 tex_coord = o_TexCoord;
    tex_coord.x = scale(tex_coord.x, o_Scale.x);
    tex_coord.y = scale(tex_coord.y, o_Scale.y);
    tex_coord.z = scale(tex_coord.z, o_Scale.z);


//    vec3 ambient = u_Light.ambient * u_Material.ambient;
    vec3 ambient = vec3(0.0);
    vec3 diffuse = u_Light.diffuse * texture(u_Texture, vec4(tex_coord, o_TexIndex)).rgb * diff;
    vec3 specular = u_Light.specular * u_Material.specular * spec;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
//    FragColor = vec4(texture(u_Texture, vec4(tex_coord, o_TexIndex)).rgb, 1.0);
}
