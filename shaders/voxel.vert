#version 430 core

layout (location = 0) in mat4 model;
layout (location = 4) in int face;
layout (location = 5) in int block;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 o_Normal;
out vec3 o_FragPos;
out vec3 o_TexCoord;
flat out int o_Face;
flat out ivec3 o_Scale;

// @formatter:off
float voxel_data[288] = {
    // position[3], normal[3], tex_coord[2]
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
};

void main()
{
    float pos_x = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 0];
    float pos_y = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 1];
    float pos_z = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 2];

    float norm_x = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 3];
    float norm_y = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 4];
    float norm_z = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 5];

    float tex_x = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 6];
    float tex_y = voxel_data[face * 8 * 6 + gl_VertexID * 8 + 7];

    vec4 voxel_position = vec4(vec3(pos_x, pos_y, pos_z) * 1.0, 1.0);
    vec3 normal = vec3(norm_x, norm_y, norm_z);

    o_Normal = mat3(transpose(inverse(model))) * normal;
    o_FragPos = vec3(model * voxel_position);
    o_TexCoord = vec3(tex_x, tex_y, block * 6 + face);
    o_Scale = ivec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
    o_Face = face;

    gl_Position = u_Projection * u_View * model * voxel_position;
}
