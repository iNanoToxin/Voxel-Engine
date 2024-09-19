#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include <glm/glm.hpp>

#include <future>
#include <thread>

#include "greedy_mesher.h"
#include "chunk_map.h"
#include "common/constants.h"
#include "common/types.h"
#include "rendering/shader.h"


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

enum block_type
{
    air_block,
    grass_block,
    dirt_block,
    sand_block,
    stone_block,
    smooth_stone_block
};

inline int foo()
{
    return 0;
}

namespace voxel_engine
{
    class chunk_map;

    class chunk
    {
    public:
    // private:
        // int32_t _noise_map[CHUNK_SIZE * CHUNK_SIZE] = {};
        // int32_t _blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE] = {};

        // std::unordered_map<glm::ivec3, int32_t> _height_map;

        std::vector<quad_data> _ssbo_data;
        GLuint _vao;
        GLuint _ssbo;


        int32_t get_terrain_height(chunk_map& _map, int32_t _x, int32_t _z) const;

        int32_t get_terrain_block(int32_t _x, int32_t _y, int32_t _z) const;

    public:
        int32_t* noise_map = nullptr;
        int32_t* blocks = nullptr;
        glm::ivec3 position;
        bool loaded = false;

        std::thread thread;

        explicit chunk(const glm::ivec3& _position);

        ~chunk();


        void generate_terrain(chunk_map& _map);

        void generate_mesh();

        void render(chunk_map& _map);

        void generate(chunk_map& _map);

        void start(chunk_map& _map);


    glm::ivec3 get_world_position(int32_t _x, int32_t _y, int32_t _z) const;
        glm::ivec3 get_world_position(const glm::ivec3& _local_position) const;
        static glm::ivec3 get_chunk_position(float32_t _x, float32_t _y, float32_t _z);
        static glm::ivec3 get_chunk_position(const glm::vec3& _world_position);
        static int32_t get_yxz_index(int32_t _x, int32_t _y, int32_t _z);;
    };
}
