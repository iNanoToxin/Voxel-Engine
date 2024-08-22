#include "chunk_map.h"

voxel_engine::chunk_map::chunk_map() {}

voxel_engine::chunk* voxel_engine::chunk_map::create_chunk(const glm::ivec3& _chunk_position)
{
    if (!_map.contains(_chunk_position))
    {
        chunk new_chunk(_chunk_position);
        // new_chunk.generate_terrain();

        _map.emplace(_chunk_position, new_chunk);
    }
    return &_map.at(_chunk_position);
}

voxel_engine::chunk* voxel_engine::chunk_map::request_chunk(const glm::ivec3& _chunk_position)
{
    if (_map.contains(_chunk_position))
    {
        return &_map.at(_chunk_position);
    }
    return nullptr;
}

void voxel_engine::chunk_map::render_chunks()
{
    for (auto it = _map.begin(); it != _map.end(); it++)
    {
        it->second.render();
    }
}
