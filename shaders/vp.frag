#version 450 core

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
flat in uint o_Face;

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
    float diff = max(dot(normalize(o_Normal), normalize(u_Light.position - o_FragPos)), 0.0);
    diff = max(diff, 0.5);

//    uint w_index = (o_Face & 2) >> 1;
//    uint h_index = 2 - (o_Face >> 2);

    vec3 tex_coord = o_TexCoord;
//    tex_coord.x = scale(tex_coord.x, o_Scale[h_index]);
//    tex_coord.y = scale(tex_coord.y, o_Scale[w_index]);

    if (o_Face == 0)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.z);
        tex_coord.y = scale(tex_coord.y, o_Scale.x);
    }
    else if (o_Face == 1)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.x);
        tex_coord.y = scale(tex_coord.y, o_Scale.z);
    }
    else if (o_Face == 2 || o_Face == 3)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.z);
        tex_coord.y = scale(tex_coord.y, o_Scale.y);
    }
    else if (o_Face == 4 || o_Face == 5)
    {
        tex_coord.x = scale(tex_coord.x, o_Scale.x);
        tex_coord.y = scale(tex_coord.y, o_Scale.y);
    }

    FragColor = vec4(u_Light.diffuse * texture(u_Texture, tex_coord).rgb * diff, 1.0);
}
