#include "chunk_map.h"

voxel_engine::chunk_map::chunk_map()
{
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

int32_t voxel_engine::chunk_map::get_height(const glm::ivec3& _world_position)
{
    if (_height_map.contains(_world_position))
    {
        return _height_map.at(_world_position);
    }

    const float32_t height0 = _noise_a.GetNoise(static_cast<float32_t>(_world_position.x) + 0.5, static_cast<float32_t>(_world_position.z) + 0.5);
    const float32_t height1 = _noise_b.GetNoise(static_cast<float32_t>(_world_position.x) + 0.5, static_cast<float32_t>(_world_position.z) + 0.5);
    const float32_t height2 = _noise_c.GetNoise(static_cast<float32_t>(_world_position.x) + 0.5, static_cast<float32_t>(_world_position.z) + 0.5);

    float32_t height = height0; // * height1 * height2;
    height = (height + 1) / 2;
    height *= 64;
    height = glm::floor(height);

    _height_map.emplace(_world_position, height);
    return height;
}

uint32_t voxel_engine::chunk_map::get_block(const glm::ivec3& _world_position)
{
    const int32_t height = get_height(_world_position);

    if (_world_position.y == height)
    {
        return block_type::grass_block;
    }
    else if (_world_position.y < height)
    {
        return block_type::stone_block;
    }
    return block_type::air_block;
}

voxel_engine::chunk* voxel_engine::chunk_map::create_chunk(const glm::ivec3& _chunk_position)
{
    if (!_map.contains(_chunk_position))
    {
        chunk new_chunk(_chunk_position);
        new_chunk.generate(this);

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
