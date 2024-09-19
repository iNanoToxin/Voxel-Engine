#pragma once
#include "game/chunk_map.h"


namespace voxel_engine::world_gen
{
    static int32_t get_terrain_height(const float32_t _x, const float32_t _z)
    {
        static FastNoiseLite noise_a;
        static FastNoiseLite noise_b;
        static FastNoiseLite noise_c;

        static bool init = false;
        if (!init)
        {
            noise_a.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise_a.SetFrequency(0.01);
            noise_a.SetSeed(1397);
            // noise_a.SetFractalOctaves(4);

            noise_b.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise_b.SetFrequency(0.05);
            noise_b.SetSeed(701);

            noise_c.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise_c.SetFrequency(0.08);
            noise_c.SetSeed(1512);
            init = true;
        }


        const float32_t height0 = noise_a.GetNoise(_x, _z);
        const float32_t height1 = noise_b.GetNoise(_x, _z);
        const float32_t height2 = noise_c.GetNoise(_x, _z);

        float32_t height = height0; // * height1 * height2;
        height = (height + 1) / 2;
        height *= 64;
        height = glm::floor(height);

        return height;
        // return 64;
    }

    static int32_t get_terrain_block(const chunk* _chunk, const int32_t _x, const int32_t _y, const int32_t _z)
    {
        const float32_t world_y = (_chunk->position.y * CHUNK_SIZE + _y) * VOXEL_SIZE;
        const float32_t height = _chunk->noise_map[_x + _z * CHUNK_SIZE];

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

    static void generate_terrain(const chunk* _chunk)
    {
        for (int32_t x = 0; x < CHUNK_SIZE; x++)
        {
            for (int32_t z = 0; z < CHUNK_SIZE; z++)
            {
                const float32_t world_x = (_chunk->position.x * CHUNK_SIZE + x) * VOXEL_SIZE;
                const float32_t world_z = (_chunk->position.z * CHUNK_SIZE + z) * VOXEL_SIZE;

                _chunk->noise_map[x + z * CHUNK_SIZE] = get_terrain_height(world_x, world_z);
            }
        }

        for (int32_t y = 0; y < CHUNK_SIZE; y++)
        {
            for (int32_t x = 0; x < CHUNK_SIZE; x++)
            {
                for (int32_t z = 0; z < CHUNK_SIZE; z++)
                {
                    _chunk->blocks[y + x * CHUNK_SIZE + z * CHUNK_SIZE_2] = get_terrain_block(_chunk, x, y, z);
                }
            }
        }
    }
}
