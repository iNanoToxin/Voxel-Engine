#pragma once
#include <unordered_map>
#include "camera.h"
#include "chunk.h"
#include "glm/glm.hpp"
#include "utilities/FastNoiseLite.h"
#include "rendering/shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <queue>

#include "glm/gtx/hash.hpp"


namespace voxel_engine
{
    class chunk;

    class chunk_map
    {
    private:
        std::unordered_map<glm::ivec3, chunk> _map;
        std::queue<glm::ivec3> _queue;
        glm::ivec3 _last_chunk_pos = glm::ivec3(INT_MAX, INT_MAX, INT_MAX);
    public:
        FastNoiseLite _noise_a;
        FastNoiseLite _noise_b;
        FastNoiseLite _noise_c;
        shader shader;

        chunk_map();

        float32_t get_noise(const float32_t _world_x, const float32_t _world_z) const;

        chunk* create_chunk(const glm::ivec3& _chunk_position);

        chunk* request_chunk(const glm::ivec3& _chunk_position);

        void render_chunks(chunk_map& _chunk_map, const glm::vec3& _world_position);

        void clear();
    };
}
