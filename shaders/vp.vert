#version 430 core

#define CHUNK_SIZE 32
#define VOXEL_SIZE 1
#define HALF_VOXEL_SIZE (VOXEL_SIZE * 0.5)

struct quad_data
{
    uint packed_data0; // x, y, z, w, h
    uint packed_data1; // face, type
};

layout (std430, binding = 0) readonly buffer vertex_pull_buffer
{
    quad_data packed_mesh_data[];
};

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 o_Normal;
out vec3 o_FragPos;
out vec3 o_TexCoord;
flat out ivec3 o_Scale;
flat out uint o_Face;

const vec3 face_offset_lookup[24] = {
    // top_face (CCW)
    vec3( HALF_VOXEL_SIZE,  0.0,  HALF_VOXEL_SIZE),
    vec3( HALF_VOXEL_SIZE,  0.0, -HALF_VOXEL_SIZE),
    vec3(-HALF_VOXEL_SIZE,  0.0, -HALF_VOXEL_SIZE),
    vec3(-HALF_VOXEL_SIZE,  0.0,  HALF_VOXEL_SIZE),
    // bottom_face (CCW)
    vec3(-HALF_VOXEL_SIZE, -0.0, -HALF_VOXEL_SIZE),
    vec3( HALF_VOXEL_SIZE, -0.0, -HALF_VOXEL_SIZE),
    vec3( HALF_VOXEL_SIZE, -0.0,  HALF_VOXEL_SIZE),
    vec3(-HALF_VOXEL_SIZE, -0.0,  HALF_VOXEL_SIZE),
    // right_face (CCW)
    vec3( 0.0, -HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE),
    vec3( 0.0, -HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE),
    vec3( 0.0,  HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE),
    vec3( 0.0,  HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE),
    // left_face (CCW)
    vec3(-0.0, -HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE),
    vec3(-0.0, -HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE),
    vec3(-0.0,  HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE),
    vec3(-0.0,  HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE),
    // front_face (CCW)
    vec3(-HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE,  0.0),
    vec3( HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE,  0.0),
    vec3( HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE,  0.0),
    vec3(-HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE,  0.0),
    // back_face (CCW)
    vec3( HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE, -0.0),
    vec3(-HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE, -0.0),
    vec3(-HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE, -0.0),
    vec3( HALF_VOXEL_SIZE,  HALF_VOXEL_SIZE, -0.0),
};

const vec3 normal_lookup[6] = {
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(-1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 1.0, -1.0),
};

const vec2 uv_coord_lookup[4] = {
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
};

// winding order to access the face positions
const int index_lookup[6] = {0, 1, 2, 2, 3, 0};
const int flip_lookup[6] = {1, -1, -1, 1, -1, 1};

void main()
{
    // create a custom index to access the mesh data in the right order
    const int index = gl_VertexID / 6;
    const int currVertexID = gl_VertexID % 6;
    const quad_data data = packed_mesh_data[index];

    // unpack the data we retrieved -> packed_data0{x:000000, y:000000, z:000000, w:000000, h:000000, 00?}
    const uint x = (data.packed_data0 >> 0) & 63;
    const uint y = (data.packed_data0 >> 6) & 63;
    const uint z = (data.packed_data0 >> 12) & 63;
    const uint w = (data.packed_data0 >> 18) & 63;
    const uint h = (data.packed_data0 >> 24) & 63;
    const uint face = (data.packed_data1) & 7;
    const uint type = (data.packed_data1 >> 3) & 255;

    const uint chunk_x = (data.packed_data1 >> 11) & 127;
    const uint chunk_y = (data.packed_data1 >> 18) & 127;
    const uint chunk_z = (data.packed_data1 >> 25) & 127;

    vec3 position = vec3(x, y, z) * VOXEL_SIZE + vec3(chunk_x, chunk_y, chunk_z) * CHUNK_SIZE * VOXEL_SIZE; // - vec3(HALF_VOXEL_SIZE);

    // get the index to update the scale's width and height according to face type
    uint w_index = (face & 2) >> 1;
    uint h_index = 2 - (face >> 2);

    vec3 offset = face_offset_lookup[face * 4 + index_lookup[currVertexID]];
    // scale the offset by width and height
    offset[w_index] *= w;
    offset[h_index] *= h;
    // translate the position back by half the amount
    offset[w_index] += VOXEL_SIZE * float(w) * 0.5 * flip_lookup[face];
    offset[h_index] += VOXEL_SIZE * float(h) * 0.5;
    // apply the offsets for our face so we can form the 2 triangles
    position += offset;

    // apply the correct scaling factor depending on face type
    o_Scale = ivec3(1, 1, 1);
    o_Scale[w_index] = int(w);
    o_Scale[h_index] = int(h);

    o_TexCoord = vec3(uv_coord_lookup[index_lookup[currVertexID]], type * 6 + face);
    o_Normal = normal_lookup[face];
    o_Face = face;
    o_FragPos = position;

    // output our position
    gl_Position = u_Projection * u_View * vec4(position, 1.0);
}
