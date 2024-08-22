#pragma once
#include <unordered_map>
#include "camera.h"
#include "chunk.h"
#include "glm/glm.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


namespace voxel_engine
{
    class chunk;

    class chunk_map
    {
    private:
        std::unordered_map<glm::ivec3, chunk> _map;

    public:
        chunk_map();

        chunk* create_chunk(const glm::ivec3& _chunk_position);
        chunk* request_chunk(const glm::ivec3& _chunk_position);

        void render_chunks();
    };
}
