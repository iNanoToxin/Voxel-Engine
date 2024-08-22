#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include <glm/glm.hpp>

#include <future>
#include <thread>

#include "greedy_mesher.h"
#include "common/constants.h"
#include "common/types.h"
#include "rendering/shader.h"
#include "rendering/gl/vertex_array.h"
#include "rendering/gl/vertex_buffer.h"
#include "utilities/FastNoiseLite.h"


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
    private:
        int32_t _noise_map[CHUNK_SIZE][CHUNK_SIZE] = {};
        int32_t _blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {};

        // std::unordered_map<glm::ivec3, int32_t> _height_map;

        std::vector<quad_data> ssbo_data;
        uint32_t vao;
        uint32_t ssbo;

        FastNoiseLite _noise_a;
        FastNoiseLite _noise_b;
        FastNoiseLite _noise_c;

        int32_t get_terrain_height(const int32_t _x, const int32_t _z) const
        {
            const float32_t world_x = position.x * CHUNK_SIZE + _x;
            const float32_t world_z = position.z * CHUNK_SIZE + _z;

            const float32_t height0 = _noise_a.GetNoise(world_x + 0.5, world_z + 0.5);
            const float32_t height1 = _noise_b.GetNoise(world_x + 0.5, world_z + 0.5);
            const float32_t height2 = _noise_c.GetNoise(world_x + 0.5, world_z + 0.5);

            float32_t height = height0; // * height1 * height2;
            height = (height + 1) / 2;
            height *= 64;
            height = glm::floor(height);

            return height;
        }

        int32_t get_terrain_block(const int32_t _x, const int32_t _y, const int32_t _z) const
        {
            const float32_t world_y = position.y * CHUNK_SIZE + _y;
            const float32_t height = _noise_map[_x][_z];

            if (world_y == height)
            {
                return block_type::grass_block;
            }
            else if (world_y < height)
            {
                return block_type::stone_block;
            }
            return block_type::air_block;
        }

    public:
        shader shader;
        glm::ivec3 position;
        explicit chunk(const glm::ivec3& _position);


        void generate_terrain()
        {
            std::vector<std::future<void>> threads;

            for (int32_t x = 0; x < CHUNK_SIZE; x++)
            {
                for (int32_t z = 0; z < CHUNK_SIZE; z++)
                {
                    threads.push_back(std::async(std::launch::async, [=](){
                        _noise_map[x][z] = get_terrain_height(x, z);
                    }));


                    // _blocks[y][x][z] = get_terrain_block(x, y, z);
                }
            }

            for (int32_t i = 0; i < threads.size(); i++)
            {
                threads[i].wait();
            }

            for (int32_t y = 0; y < CHUNK_SIZE; y++)
            {
                for (int32_t x = 0; x < CHUNK_SIZE; x++)
                {
                    for (int32_t z = 0; z < CHUNK_SIZE; z++)
                    {


                        _blocks[y][x][z] = get_terrain_block(x, y, z);
                    }
                }
            }
        }

        void generate_mesh();

        void render();




        glm::ivec3 get_world_position(int32_t _x, int32_t _y, int32_t _z) const;
        glm::ivec3 get_world_position(const glm::ivec3& _local_position) const;
        static glm::ivec3 get_chunk_position(float32_t _x, float32_t _y, float32_t _z);
        static glm::ivec3 get_chunk_position(const glm::vec3& _world_position);
        static int32_t get_yxz_index(int32_t _x, int32_t _y, int32_t _z);;
    };
}
