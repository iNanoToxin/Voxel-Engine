#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include <glm/glm.hpp>

#include "chunk_map.h"
#include "greedy_mesher.h"
#include "common/constants.h"
#include "common/types.h"


struct quad_data
{
    uint32_t packed_data0; // x, y, z, w, h
    uint32_t packed_data1; // face, type
};

enum face_type
{
    left_face,
    right_face,
    bottom_face,
    top_face,
    back_face,
    front_face,
};

enum block_type : uint32_t
{
    air_block,
    grass_block,
    dirt_block,
    sand_block,
    stone_block,
    smooth_stone_block
};

namespace voxel_engine
{
    class chunk_map;

    class chunk
    {
    private:
        uint32_t _blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {};
        uint32_t _noise_map[CHUNK_SIZE][CHUNK_SIZE] = {};

    public:
        glm::ivec3 position;
        explicit chunk(const glm::ivec3& _position);

        void generate(chunk_map* _map);

        std::vector<quad_data> generate_mesh(chunk_map* _map) const;


        glm::ivec3 get_world_position(uint32_t _x, uint32_t _y, uint32_t _z) const;
        glm::ivec3 get_world_position(const glm::uvec3& _local_position) const;
        static glm::ivec3 get_chunk_position(float32_t _x, float32_t _y, float32_t _z);
        static glm::ivec3 get_chunk_position(const glm::vec3& _world_position);
        static uint32_t get_yxz_index(uint32_t _x, uint32_t _y, uint32_t _z);;
    };
}
