#include "chunk.h"

voxel_engine::chunk::chunk(const glm::ivec3& _position)
    : _position(_position)
{
    _noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    _noise.SetFrequency(0.011254);
    _noise.SetSeed(1234);

    for (uint32_t x = 0; x < CHUNK_SIZE; x++)
    {
        for (uint32_t z = 0; z < CHUNK_SIZE; z++)
        {
            float32_t height = _noise.GetNoise(static_cast<float32_t>(_position.x * CHUNK_SIZE + x), static_cast<float32_t>(_position.z * CHUNK_SIZE + z));
            height = (height + 1) / 2;
            height *= 32;
            height = glm::floor(height);

            _noise_map[x][z] = height;
        }
    }

    for (uint32_t x = 0; x < CHUNK_SIZE; x++)
    {
        for (uint32_t z = 0; z < CHUNK_SIZE; z++)
        {
            for (uint32_t y = 0; y < CHUNK_SIZE; y++)
            {
                if (y == _noise_map[x][z])
                {
                    _blocks[x][z][y] = block_type::grass_block;
                }
                else if (y < _noise_map[x][z])
                {
                    _blocks[x][z][y] = block_type::stone_block;
                }
            }
        }
    }
}

std::vector<block> voxel_engine::chunk::get_voxels() const
{
    std::vector<block> blocks;

    for (uint32_t x = 0; x < CHUNK_SIZE; x++)
    {
        for (uint32_t z = 0; z < CHUNK_SIZE; z++)
        {
            for (uint32_t y = 0; y < CHUNK_SIZE; y++)
            {
                if (_blocks[x][z][y] == block_type::air_block)
                {
                    continue;
                }

                glm::mat4 model(1.0);
                model = glm::translate(model, glm::vec3(_position * CHUNK_SIZE) + glm::vec3(x, y, z) + glm::vec3(0.5));

                blocks.emplace_back(
                    block{
                        .transform = model,
                        .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                    }
                );
            }
        }
    }
    return blocks;
}

std::vector<block> voxel_engine::chunk::get_voxels_greedy()
{
    std::vector<block> blocks;

    for (uint32_t x = 0; x < CHUNK_SIZE; x++)
    {
        for (uint32_t z = 0; z < CHUNK_SIZE; z++)
        {
            for (uint32_t y = 0; y < CHUNK_SIZE; y++)
            {
                if (_blocks[x][z][y] == block_type::air_block)
                {
                    continue;
                }

                uint32_t w_x = 1;
                uint32_t h_y = 1;
                uint32_t l_z = 1;

                while (x + w_x < CHUNK_SIZE && _blocks[x + w_x][z][y] == _blocks[x][z][y])
                {
                    _blocks[x + w_x][z][y] = block_type::air_block;
                    w_x++;
                }

                while (z + l_z < CHUNK_SIZE && _blocks[x][z + l_z][y] == _blocks[x][z][y])
                {
                    bool pass = true;
                    for (uint32_t i = 0; i < w_x; i++)
                    {
                        if (_blocks[x + i][z + l_z][y] != _blocks[x][z][y])
                        {
                            pass = false;
                            break;
                        }
                    }
                    if (!pass) break;

                    for (uint32_t i = 0; i < w_x; i++)
                    {
                        _blocks[x + i][z + l_z][y] = block_type::air_block;
                    }
                    l_z++;
                }

                while (y + h_y < CHUNK_SIZE && _blocks[x][z][y + h_y] == _blocks[x][z][y])
                {
                    bool pass = true;
                    for (uint32_t i = 0; i < w_x; i++)
                    {
                        for (uint32_t j = 0; j < l_z; j++)
                        {
                            if (_blocks[x + i][z + j][y + h_y] != _blocks[x][z][y])
                            {
                                pass = false;
                                break;
                            }
                        }
                        if (!pass) break;
                    }
                    if (!pass) break;

                    for (uint32_t i = 0; i < w_x; i++)
                    {
                        for (uint32_t j = 0; j < l_z; j++)
                        {
                            _blocks[x + i][z + j][y + h_y] = block_type::air_block;
                        }
                    }
                    h_y++;
                }


                const float32_t pos_x = (x * w_x + w_x * (w_x - 1.0) / 2.0) / w_x;
                const float32_t pos_y = (y * h_y + h_y * (h_y - 1.0) / 2.0) / h_y;
                const float32_t pos_z = (z * l_z + l_z * (l_z - 1.0) / 2.0) / l_z;

                glm::mat4 model(1.0);
                model = glm::translate(model, glm::vec3(_position * CHUNK_SIZE) + glm::vec3(pos_x, pos_y, pos_z) + glm::vec3(0.5));
                model = glm::scale(model, glm::vec3(w_x, h_y, l_z));

                blocks.emplace_back(
                    block{
                        .transform = model,
                        .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                    }
                );
                _blocks[x][z][y] = block_type::air_block;
            }
        }
    }
    return blocks;
}

std::vector<face> voxel_engine::chunk::get_voxel_faces() const
{
    std::vector<face> faces;

    for (uint32_t x = 0; x < CHUNK_SIZE; x++)
    {
        for (uint32_t z = 0; z < CHUNK_SIZE; z++)
        {
            for (uint32_t y = 0; y < CHUNK_SIZE; y++)
            {
                if (_blocks[x][z][y] == block_type::air_block)
                {
                    continue;
                }

                glm::mat4 model(1.0);
                model = glm::translate(model, glm::vec3(_position * CHUNK_SIZE) + glm::vec3(x, y, z) + glm::vec3(0.5));

                /*for (int32_t axis = face_type::left_face; axis <= face_type::front_face; axis++)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = axis,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }
                {
                    continue;
                }*/

                if (x + 1 >= CHUNK_SIZE || _blocks[x + 1][z][y] == block_type::air_block)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = face_type::right_face,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }
                if (x == 0 || _blocks[x - 1][z][y] == block_type::air_block)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = face_type::left_face,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }

                if (y + 1 >= CHUNK_SIZE || _blocks[x][z][y + 1] == block_type::air_block)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = face_type::top_face,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }
                if (y == 0 || _blocks[x][z][y - 1] == block_type::air_block)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = face_type::bottom_face,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }

                if (z + 1 >= CHUNK_SIZE || _blocks[x][z + 1][y] == block_type::air_block)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = face_type::front_face,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }
                if (z == 0 || _blocks[x][z - 1][y] == block_type::air_block)
                {
                    faces.push_back(
                        face{
                            .transform = model,
                            .face_type = face_type::back_face,
                            .block_type = static_cast<int32_t>(_blocks[x][z][y] - 1)
                        }
                    );
                }
            }
        }
    }
    return faces;
}
