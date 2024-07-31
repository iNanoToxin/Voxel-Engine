#pragma once
#include <array>
#include <iomanip>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <bitset>

#include "common/constants.h"
#include "common/types.h"
#include "utilities/FastNoiseLite.h"

typedef std::bitset<32> bit32_t;

enum face_type
{
    left_face,
    right_face,
    bottom_face,
    top_face,
    back_face,
    front_face
};

struct face
{
    glm::mat4 transform;
    int32_t face_type;
    int32_t block_type;
};

struct block
{
    glm::mat4 transform;
    int32_t block_type;
};

enum block_type : uint32_t
{
    air_block,
    grass_block,
    dirt_block,
    sand_block,
    stone_block,
    smooth_stone_block
};

struct quad
{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
};

inline std::vector<quad> binary_mesh_plane(uint32_t _data[CHUNK_SIZE])
{
    std::vector<quad> quads;

    /*for (uint32_t row = 0; row < 32; row++)
    {
        std::cout << std::bitset<32>(_data[row]) << std::endl;
    }
    std::cout << std::endl;*/

    for (uint32_t row = 0; row < 32; row++)
    {
        // repeats until all bits are cleared
        while (_data[row] != 0)
        {
            /*
             * 111111111111111110000000111(00000) : trailing_zeros = 5
             * 111111111111111110000000(111)00000 : width = 3
             * 000000000000000000000000(111)00000 : width_mask
            **/
            const uint32_t trailing_zeros = __builtin_ctz(_data[row]);
            const uint32_t width = __builtin_ctz(~(_data[row] >> trailing_zeros));
            const uint32_t width_mask = ~_data[row] == 0u ? ~0u : ((1u << width) - 1u) << trailing_zeros;

            // std::cout << "trailing_zeros: " << std::setw(2) << trailing_zeros << std::endl;
            // std::cout << "width_mask: " << std::bitset<32>(((1U << width) - 1)) << std::endl;

            uint32_t height = 0;
            /*
             *   000000000000000000000000(111)00000 : width_mask
             * & 111111111111111110000000(111)00000 : data[row]
             * ------------------------------------
             * = 000000000000000000000000(111)00000 : width_mask
             *
             * 111111111111111110000000(111)00000 : height = 1
             * 111111111111111100000000(111)00000 : height = 2
             * 111111111111111000000000(111)00000 : height = 3
            **/

            // check if next row has same bits as width_mask
            while (row + height < 32u && (width_mask & _data[row + height]) == width_mask)
            {
                // clear bits used by width_mask
                _data[row + height] ^= width_mask;
                height++;
            }

            // std::cout << "x: " << std::setw(2) << row << " ";
            // std::cout << "y: " << std::setw(2) << trailing_zeros << " ";
            // std::cout << "w: " << std::setw(2) << width << " ";
            // std::cout << "h: " << std::setw(2) << height << " ";
            // std::cout << std::endl;

            quads.push_back(quad{
                .x = trailing_zeros,
                .y = row,
                .w = width,
                .h = height
            });
            // break;
        }
    }
    return quads;
}

inline glm::vec3 get_face_scale(const uint32_t _face, const quad& _quad)
{
    switch (_face)
    {
        case face_type::left_face:
        case face_type::right_face:
        {
            return glm::vec3(1.0, _quad.h, _quad.w);
        }
        case face_type::bottom_face:
        case face_type::top_face:
        {
            return glm::vec3(_quad.h, 1.0, _quad.w);
        }
        case face_type::back_face:
        case face_type::front_face:
        {
            return glm::vec3(_quad.w, _quad.h, 1.0);
        }
        default: break;
    }
    return {};
}

inline glm::vec3 get_face_position(const uint32_t _face, const quad& _quad, const uint32_t _depth)
{
    const glm::vec3 scale = get_face_scale(_face, _quad);
    glm::vec3 position{};

    switch (_face)
    {
        case face_type::left_face:
        case face_type::right_face:
        {
            position.x = _depth;
            position.y = (_quad.y * scale.y + scale.y * (scale.y - 1.0) / 2.0) / scale.y;
            position.z = (_quad.x * scale.z + scale.z * (scale.z - 1.0) / 2.0) / scale.z;
            break;
        }
        case face_type::bottom_face:
        case face_type::top_face:
        {
            position.x = (_quad.y * scale.x + scale.x * (scale.x - 1.0) / 2.0) / scale.x;
            position.y = _depth;
            position.z = (_quad.x * scale.z + scale.z * (scale.z - 1.0) / 2.0) / scale.z;
            break;
        }
        case face_type::back_face:
        case face_type::front_face:
        {
            position.x = (_quad.x * scale.x + scale.x * (scale.x - 1.0) / 2.0) / scale.x;
            position.y = (_quad.y * scale.y + scale.y * (scale.y - 1.0) / 2.0) / scale.y;
            position.z = _depth;
            break;
        }
        default: break;
    }
    return position;
}


namespace voxel_engine
{
    class chunk
    {
    private:
        uint32_t _blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {};
        uint32_t _noise_map[CHUNK_SIZE][CHUNK_SIZE] = {};
        glm::ivec3 _position;

        FastNoiseLite _noise;
        uint32_t _axis_bits[3][CHUNK_SIZE][CHUNK_SIZE] {};
        uint32_t _face_bits[6][CHUNK_SIZE][CHUNK_SIZE] {};
    public:
        explicit chunk(const glm::ivec3& _position);

        std::vector<block> get_voxels() const;
        std::vector<block> get_voxels_greedy();
        std::vector<face> get_voxel_faces() const;


        std::vector<face> get_voxel_faces_greedy()
        {
            std::vector<face> faces;

            // Complexity: GenAxis(32 * 32 * 32) + CullFace(32 * 64) + CreateFace(32 * 6 * N)

            for (uint32_t x = 0; x < CHUNK_SIZE; x++)
            {
                for (uint32_t y = 0; y < CHUNK_SIZE; y++)
                {
                    for (uint32_t z = 0; z < CHUNK_SIZE; z++)
                    {
                        // slices on x-axis -> contains y with respect to z
                        _axis_bits[0][x][y] |= (1 << z) * (_blocks[x][z][y] != block_type::air_block);
                        // slices on z-axis -> contains y with respect to x
                        _axis_bits[1][x][y] |= (1 << z) * (_blocks[y][z][x] != block_type::air_block);
                        // slices on y-axis -> contains x with respect to z
                        _axis_bits[2][x][y] |= (1 << z) * (_blocks[z][x][y] != block_type::air_block);
                    }
                }
            }

            for (uint32_t y = 0; y < CHUNK_SIZE; y++)
            {
                uint32_t mask0, mask1;
                uint32_t l_slice = 0;
                uint32_t r_slice = CHUNK_SIZE - 1;

                // sets inital chunk border faces
                _face_bits[face_type::left_face][l_slice][y] = _axis_bits[0][l_slice][y];
                _face_bits[face_type::right_face][r_slice][y] = _axis_bits[0][r_slice][y];
                _face_bits[face_type::bottom_face][l_slice][y] = _axis_bits[1][l_slice][y];
                _face_bits[face_type::top_face][r_slice][y] = _axis_bits[1][r_slice][y];
                _face_bits[face_type::back_face][l_slice][y] = _axis_bits[2][l_slice][y];
                _face_bits[face_type::front_face][r_slice][y] = _axis_bits[2][r_slice][y];

                // hides faces next to blocks and show faces next to air
                while (l_slice < CHUNK_SIZE - 1)
                {
                    /*
                     *   11111111111111111(1111111)11111111 : mask0
                     * ^ 11111111111111111(0000000)11111111 : mask1
                     * ------------------------------------
                     * = 00000000000000000(1111111)00000000 : mask0 ^ mask1
                     * & 11111111111111111(1111111)11111111 : mask0
                     * ------------------------------------
                     * = 00000000000000000(1111111)00000000 : (mask0 ^ mask1) & mask0
                    **/
                    mask0 = _axis_bits[0][l_slice][y];
                    mask1 = _axis_bits[0][l_slice + 1][y];
                    // creates mask for right_face
                    _face_bits[face_type::right_face][l_slice][y] = (mask0 ^ mask1) & mask0;

                    mask0 = _axis_bits[1][l_slice][y];
                    mask1 = _axis_bits[1][l_slice + 1][y];
                    // creates mask for top_face
                    _face_bits[face_type::top_face][l_slice][y] = (mask0 ^ mask1) & mask0;

                    mask0 = _axis_bits[2][l_slice][y];
                    mask1 = _axis_bits[2][l_slice + 1][y];
                    // creates mask for front_face
                    _face_bits[face_type::front_face][l_slice][y] = (mask0 ^ mask1) & mask0;

                    l_slice++;
                }

                // hides faces next to blocks and show faces next to air
                while (r_slice > 0)
                {
                    mask0 = _axis_bits[0][r_slice][y];
                    mask1 = _axis_bits[0][r_slice - 1][y];
                    // creates mask for left_face
                    _face_bits[face_type::left_face][r_slice][y] = (mask0 ^ mask1) & mask0;

                    mask0 = _axis_bits[1][r_slice][y];
                    mask1 = _axis_bits[1][r_slice - 1][y];
                    // creates mask for bottom_face
                    _face_bits[face_type::bottom_face][r_slice][y] = (mask0 ^ mask1) & mask0;

                    mask0 = _axis_bits[2][r_slice][y];
                    mask1 = _axis_bits[2][r_slice - 1][y];
                    // creates mask for back_face
                    _face_bits[face_type::back_face][r_slice][y] = (mask0 ^ mask1) & mask0;

                    r_slice--;
                }
            }

            for (uint32_t slice_pos = 0; slice_pos < CHUNK_SIZE; slice_pos++)
            {
                for (int32_t face = 0; face < 6; face++)
                {
                    std::vector<quad> mesh = binary_mesh_plane(_face_bits[face][slice_pos]);

                    for (const quad& quad : mesh)
                    {
                        const glm::vec3 scale = get_face_scale(face, quad);
                        glm::vec3 pos = get_face_position(face, quad, slice_pos);

                        pos += glm::vec3(_position * CHUNK_SIZE);
                        pos += glm::vec3(0.5);
                        pos += glm::vec3(0.0, 0.0, 16.0 * 3);

                        glm::mat4 model(1.0);
                        model = glm::translate(model, pos);
                        model = glm::scale(model, scale);

                        faces.push_back({
                            .transform = model,
                            .face_type = face,
                            .block_type = 0
                        });
                    }
                }
            }

            return faces;
        }
    };
}
