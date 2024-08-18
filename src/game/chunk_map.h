#pragma once
#include <unordered_map>
#include "chunk.h"
#include "glm/glm.hpp"
#include "utilities/FastNoiseLite.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


namespace voxel_engine
{
    class chunk;

    class chunk_map
    {
    private:
        std::unordered_map<glm::ivec3, chunk> _map;
        std::unordered_map<glm::ivec3, int32_t> _height_map;

        FastNoiseLite _noise_a;
        FastNoiseLite _noise_b;
        FastNoiseLite _noise_c;
    public:
        chunk_map();

        int32_t get_height(const glm::ivec3& _world_position);
        uint32_t get_block(const glm::ivec3& _world_position);

        chunk* create_chunk(const glm::ivec3& _chunk_position);
        chunk* request_chunk(const glm::ivec3& _chunk_position);
    };
}
