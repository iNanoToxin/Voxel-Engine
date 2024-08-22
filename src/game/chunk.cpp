#include "chunk.h"
#include "camera.h"

voxel_engine::chunk::chunk(const glm::ivec3& _position) : shader("vp.vert", "vp.frag"), position(_position)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ssbo);

    _noise_a.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    _noise_a.SetFrequency(0.01);
    _noise_a.SetSeed(1397);
    // noise_a.SetFractalOctaves(4);

    _noise_b.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    _noise_b.SetFrequency(0.05);
    _noise_b.SetSeed(701);

    _noise_c.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    _noise_c.SetFrequency(0.08);
    _noise_c.SetSeed(1512);
}

void voxel_engine::chunk::generate_mesh()
{
    voxel_engine::mesh_data mesh_data;
    mesh_data.opaque_mask = new uint64_t[CHUNK_SIZE_PADDED_2]{};
    mesh_data.face_masks = new uint64_t[CHUNK_SIZE_2 * 6]{};
    mesh_data.forward_merged = new uint8_t[CHUNK_SIZE_2]{};
    mesh_data.right_merged = new uint8_t[CHUNK_SIZE]{};
    mesh_data.vertices = new std::vector<uint64_t>(10000);
    mesh_data.max_vertices = 10000;

    uint8_t* voxels = new uint8_t[CHUNK_SIZE_PADDED_3]{};
    memset(voxels, 0, CHUNK_SIZE_PADDED_3);
    memset(mesh_data.opaque_mask, 0, CHUNK_SIZE_PADDED_2 * sizeof(uint64_t));

    for (int y = 0; y < CHUNK_SIZE_PADDED; y++)
    {
        for (int x = 0; x < CHUNK_SIZE_PADDED; x++)
        {
            for (int z = 0; z < CHUNK_SIZE_PADDED; z++)
            {
                if (y > 0 && x > 0 && z > 0 && y < CHUNK_SIZE_PADDED - 1 && x < CHUNK_SIZE_PADDED - 1 && z < CHUNK_SIZE_PADDED - 1)
                {
                    if (_blocks[y - 1][x - 1][z - 1] == block_type::air_block)
                    {
                        continue;
                    }
                    voxels[get_yxz_index(x, y, z)] = _blocks[y - 1][x - 1][z - 1];
                    mesh_data.opaque_mask[y * CHUNK_SIZE_PADDED + x] |= 1ull << z;
                }
                // else if (get_terrain_block(x, y, z) != block_type::air_block)
                // {
                //     mesh_data.opaque_mask[y * CHUNK_SIZE_PADDED + x] |= 1ull << z;
                // }
            }
        }
    }

    voxel_engine::greedy_mesher::mesh(voxels, mesh_data);

    ssbo_data.clear();

    for (int32_t face = 0; face < 6; face++)
    {
        const int32_t vertex_begin = mesh_data.face_vertex_begin[face];
        const int32_t vertex_length = mesh_data.face_vertex_length[face];

        for (int32_t i = vertex_begin; i < vertex_begin + vertex_length; i++)
        {
            const uint64_t quad = mesh_data.vertices->at(i);

            // pack the data -> packed_data0{x:000000, y:000000, z:000000, w:000000, h:000000, 00?}
            // pack the data -> packed_data1{face:000, type:00000000, cx:0000000, cy:0000000 cz:0000000}
            const uint8_t x = quad & 63u;
            const uint8_t y = (quad >> 6u) & 63u;
            const uint8_t z = (quad >> 12u) & 63u;
            const uint8_t w = (quad >> 18u) & 63u;
            const uint8_t h = (quad >> 24u) & 63u;
            const uint8_t type = (quad >> 32u) & 63u;

            if (type == block_type::air_block)
            {
                continue;
            }

            const uint32_t vertex_data0 = (x) | (y << 6) | (z << 12) | (w << 18) | (h << 24);
            const uint32_t vertex_data1 = (face) | ((type - 1) << 3) | (position.x << 11) | (position.y << 18) | (position.z << 25);
            ssbo_data.push_back(
                quad_data{
                    .packed_data0 = vertex_data0,
                    .packed_data1 = vertex_data1
                }
            );
        }
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_data.size() * sizeof(quad_data), ssbo_data.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // glBindVertexArray(0);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // vao.bind_vertex_array();
    // ssbo.bind_buffer();
    // ssbo.set_buffer_data(ssbo_data.size() * sizeof(quad_data), ssbo_data.data(), GL_STATIC_DRAW);
    // ssbo.bind_buffer_base(0);

    delete voxels;
    delete mesh_data.opaque_mask;
    delete mesh_data.face_masks;
    delete mesh_data.forward_merged;
    delete mesh_data.right_merged;
    delete mesh_data.vertices;
}

void voxel_engine::chunk::render()
{
    shader.use();
    shader.set_mat4("u_View", camera::get_current_camera()->get_view_matrix());
    shader.set_mat4("u_Projection", camera::get_current_camera()->get_projection_matrix());
    shader.set_vec3("u_ViewPos", camera::get_current_camera()->position);

    // vp_shader.set_vec3("u_Light.position", _camera.position + glm::vec3(0.0f, 100.0f, 0.0f));
    shader.set_vec3("u_Light.position", glm::vec3(250.0, 1000.0, 750.0) * 10000.0f);
    shader.set_vec3("u_Light.ambient", glm::vec3(0.5f));
    shader.set_vec3("u_Light.diffuse", glm::vec3(1.0f));
    shader.set_vec3("u_Light.specular", glm::vec3(1.0f));
    shader.set_vec3("u_Material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.set_vec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.set_float32("u_Material.shininess", 64.0f);
    //
    //
    //
    // vao.bind_vertex_array();
    // vao.draw_arrays(GL_TRIANGLES, 0, ssbo_data.size() * 6);

    glBindVertexArray(vao);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_data.size() * sizeof(quad_data), ssbo_data.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glDrawArrays(GL_TRIANGLES, 0, ssbo_data.size() * 6);

    // glBindVertexArray(0);
    // glUseProgram(0);
}

glm::ivec3 voxel_engine::chunk::get_world_position(const int32_t _x, const int32_t _y, const int32_t _z) const
{
    return glm::ivec3(
        position.x * CHUNK_SIZE + _x,
        position.y * CHUNK_SIZE + _y,
        position.z * CHUNK_SIZE + _z
    );
}

glm::ivec3 voxel_engine::chunk::get_world_position(const glm::ivec3& _local_position) const
{
    return get_world_position(_local_position.x, _local_position.y, _local_position.z);
}

glm::ivec3 voxel_engine::chunk::get_chunk_position(const float32_t _x, const float32_t _y, const float32_t _z)
{
    const int32_t chunk_pos_x = glm::floor(_x / CHUNK_SIZE);
    const int32_t chunk_pos_y = glm::floor(_y / CHUNK_SIZE);
    const int32_t chunk_pos_z = glm::floor(_z / CHUNK_SIZE);
    return glm::ivec3(chunk_pos_x, chunk_pos_y, chunk_pos_z);
}

glm::ivec3 voxel_engine::chunk::get_chunk_position(const glm::vec3& _world_position)
{
    return get_chunk_position(_world_position.x, _world_position.y, _world_position.z);
}

int32_t voxel_engine::chunk::get_yxz_index(const int32_t _x, const int32_t _y, const int32_t _z)
{
    return _z + (_x * CHUNK_SIZE_PADDED) + (_y * CHUNK_SIZE_PADDED_2);
}
