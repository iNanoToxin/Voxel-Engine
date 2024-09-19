#include "chunk_map.h"
#include "world_gen.h"

voxel_engine::chunk_map::chunk_map()
    : shader("vp.vert", "vp.frag")
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

float32_t voxel_engine::chunk_map::get_noise(const float32_t _world_x, const float32_t _world_z) const
{
    const float32_t height0 = _noise_a.GetNoise(_world_x + 0.5, _world_z + 0.5);
    const float32_t height1 = _noise_b.GetNoise(_world_x + 0.5, _world_z + 0.5);
    const float32_t height2 = _noise_c.GetNoise(_world_x + 0.5, _world_z + 0.5);

    float32_t height = height0; // * height1 * height2;
    height = (height + 1) / 2;
    height *= 64;
    height = glm::floor(height);

    return height;
}

voxel_engine::chunk* voxel_engine::chunk_map::create_chunk(const glm::ivec3& _chunk_position)
{
    if (!_map.contains(_chunk_position))
    {
        // avoid creating duplicate chunks due to unordered_map internals
        _map.emplace(_chunk_position, _chunk_position);
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

void voxel_engine::chunk_map::render_chunks(chunk_map& _chunk_map, const glm::vec3& _world_position)
{
    const glm::ivec3 chunk_pos = chunk::get_chunk_position(_world_position);

    if (_last_chunk_pos != chunk_pos)
    {
        _last_chunk_pos = chunk_pos;
        _queue = {};

        for (int32_t y = 0; y < 2; y++)
        {
            for (int32_t x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++)
            {
                for (int32_t z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++)
                {
                    glm::ivec3 pos = glm::ivec3(chunk_pos.x + x, y, chunk_pos.z + z);

                    if (request_chunk(pos) == nullptr)
                    {
                        _queue.push(pos);
                    }
                }
            }
        }
    }

    if (!_queue.empty())
    {
        if (request_chunk(_queue.front()) == nullptr)
        {
            voxel_engine::chunk* chunk = create_chunk(_queue.front());
            // chunk->generate_terrain(*this);
            // chunk->generate_mesh();
            // std::thread thread_obj(&chunk::generate, chunk, *this);

            // auto f = [=]() {
            //     chunk->generate_terrain(*this);
            //     chunk->generate_mesh();
            //     chunk->loaded = true;
            // };
            //
            // std::thread thread_obj(f);
            // thread_obj.join();
            //
            // glNamedBufferData(chunk->_ssbo, chunk->_ssbo_data.size() * sizeof(quad_data), chunk->_ssbo_data.data(), GL_STATIC_DRAW);

            chunk->start(*this);
        }
        _queue.pop();
    }

    auto it = _map.begin();

    while (it != _map.end())
    {
        if (glm::abs(it->second.position.x - chunk_pos.x) > RENDER_DISTANCE || glm::abs(it->second.position.z - chunk_pos.z) > RENDER_DISTANCE)
        {
            it = _map.erase(it);
            continue;
        }

        if (it->second.loaded)
        {
            glNamedBufferData(it->second._ssbo, it->second._ssbo_data.size() * sizeof(quad_data), it->second._ssbo_data.data(), GL_STATIC_DRAW);
            it->second.render(_chunk_map);
        }
        it++;
    }
}

void voxel_engine::chunk_map::clear()
{
    _map.clear();
}
