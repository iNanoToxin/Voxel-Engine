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
flat in int o_Face;
//flat in int o_TexIndex;
//flat in int o_VertexId;



uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;
uniform sampler2DArray u_Texture;

float scale(float coord, float scale)
{
    return mod(coord * scale, 1.0);
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

    if (o_Face == 0 || o_Face == 1)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.z);
        tex_coord.y = scale(tex_coord.y, o_Scale.y);
    }
    else if (o_Face == 2 || o_Face == 3)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.x);
        tex_coord.y = scale(tex_coord.y, o_Scale.z);
    }
    else if (o_Face == 4 || o_Face == 5)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.x);
        tex_coord.y = scale(tex_coord.y, o_Scale.y);
    }


//    vec3 ambient = u_Light.ambient * u_Material.ambient;
    vec3 ambient = vec3(0.0);
    vec3 diffuse = u_Light.diffuse * texture(u_Texture, tex_coord).rgb * diff;
    vec3 specular = u_Light.specular * u_Material.specular * spec;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
