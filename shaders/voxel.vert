#version 430 core
layout (location = 0) in mat4 a_Model;
layout (location = 4) in int a_TexIndex;

out vec3 o_Normal;
out vec3 o_FragPos;
out vec3 o_TexCoord;
flat out ivec3 o_Scale;
flat out int o_TexIndex;
flat out int o_VertexId;

uniform mat4 u_View;
uniform mat4 u_Projection;

// @formatter:off
float voxel_data[288] = {
    // position[3], normal[3], tex_coord[2]
    // back-face (CCW winding)
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,    // bottom-left
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,    // bottom-right
   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,    // top-right
   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,    // top-right
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,    // top-left
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,    // bottom-left
    // front-face (CCW winding)
   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,    // bottom-left
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,    // bottom-right
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,    // top-right
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,    // top-right
   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,    // top-left
   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,    // bottom-left
    // left-face (CCW winding)
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,    // top-left
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
    // right-face (CCW winding)
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,    // top-left
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
    // bottom-face (CCW winding)
   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,    // top-right
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,    // top-right
   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,    // top-left
   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
    // top-face (CCW winding)
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,    // top-right
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,    // top-right
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,    // top-left
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
};
// @formatter:on

void main()
{
    float position_x = voxel_data[gl_VertexID * 8 + 0];
    float position_y = voxel_data[gl_VertexID * 8 + 1];
    float position_z = voxel_data[gl_VertexID * 8 + 2];

    float normal_x = voxel_data[gl_VertexID * 8 + 3];
    float normal_y = voxel_data[gl_VertexID * 8 + 4];
    float normal_z = voxel_data[gl_VertexID * 8 + 5];

    float texcoord_x = voxel_data[gl_VertexID * 8 + 6];
    float texcoord_y = voxel_data[gl_VertexID * 8 + 7];

    vec4 position = vec4(position_x, position_y, position_z, 1.0);
    vec3 normal = vec3(normal_x, normal_y, normal_z);
//    vec2 tex_coord = vec2(texcoord_x, texcoord_y);
    vec3 tex_coord = vec3(position_x, position_y, position_z);

    o_TexIndex = a_TexIndex;
    o_VertexId = gl_VertexID;

    gl_Position = u_Projection * u_View * a_Model * position;
    o_Normal = mat3(transpose(inverse(a_Model))) * normal;
    o_FragPos = vec3(a_Model * position);
    o_TexCoord = tex_coord;
    o_Scale = ivec3(length(a_Model[0].xyz), length(a_Model[1].xyz), length(a_Model[2].xyz));
}
