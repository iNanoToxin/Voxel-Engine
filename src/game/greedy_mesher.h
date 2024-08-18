#pragma once
#include <vector>
#include "common/constants.h"
#include "common/types.h"

// from: https://github.com/cgerikj/binary-greedy-meshing/tree/master

namespace voxel_engine
{
    struct mesh_data
    {
        uint64_t* face_masks = nullptr;    // CHUNK_SIZE_2 * 6
        uint64_t* opaque_mask = nullptr;   // CHUNK_SIZE_PADDED_2
        uint8_t* forward_merged = nullptr; // CHUNK_SIZE_2
        uint8_t* right_merged = nullptr;   // CHUNK_SIZE
        std::vector<uint64_t>* vertices = nullptr;
        int vertex_count = 0;
        int max_vertices = 0;
        int face_vertex_begin[6] = {};
        int face_vertex_length[6] = {};
    };

    class greedy_mesher
    {
    private:
        static int get_axis_index(const int _axis, const int _a, const int _b, const int _c)
        {
            if (_axis == 0)
            {
                return _b + (_a * CHUNK_SIZE_PADDED) + (_c * CHUNK_SIZE_PADDED_2);
            }
            else if (_axis == 1)
            {
                return _b + (_c * CHUNK_SIZE_PADDED) + (_a * CHUNK_SIZE_PADDED_2);
            }
            return _c + (_a * CHUNK_SIZE_PADDED) + (_b * CHUNK_SIZE_PADDED_2);
        }

        static void insert_quad(std::vector<uint64_t>& _vertices, const uint64_t _quad, int& _vertex_i, int& _max_vertices)
        {
            if (_vertex_i >= _max_vertices - 6)
            {
                _vertices.resize(_max_vertices * 2, 0);
                _max_vertices *= 2;
            }
            _vertices[_vertex_i] = _quad;
            _vertex_i++;
        }

        static uint64_t get_quad(const uint64_t _x, const uint64_t _y, const uint64_t _z, const uint64_t _w, const uint64_t _h, const uint64_t _type)
        {
            return (_type << 32) | (_h << 24) | (_w << 18) | (_z << 12) | (_y << 6) | _x;
        }

    public:
        static void mesh(const uint8_t* _voxels, mesh_data& _mesh_data)
        {
            _mesh_data.vertex_count = 0;
            int vertex_i = 0;

            const uint64_t* opaque_mask = _mesh_data.opaque_mask;
            uint64_t* face_masks = _mesh_data.face_masks;
            uint8_t* forward_merged = _mesh_data.forward_merged;
            uint8_t* right_merged = _mesh_data.right_merged;

            // Hidden face culling
            for (int a = 1; a < CHUNK_SIZE_PADDED - 1; a++)
            {
                const int a_cs_p = a * CHUNK_SIZE_PADDED;

                for (int b = 1; b < CHUNK_SIZE_PADDED - 1; b++)
                {
                    const uint64_t column_bits = opaque_mask[(a * CHUNK_SIZE_PADDED) + b] & PADDING_MASK;
                    const int ba_index = (b - 1) + (a - 1) * CHUNK_SIZE;
                    const int ab_index = (a - 1) + (b - 1) * CHUNK_SIZE;

                    face_masks[ba_index + 0 * CHUNK_SIZE_2] = (column_bits & ~opaque_mask[a_cs_p + CHUNK_SIZE_PADDED + b]) >> 1;
                    face_masks[ba_index + 1 * CHUNK_SIZE_2] = (column_bits & ~opaque_mask[a_cs_p - CHUNK_SIZE_PADDED + b]) >> 1;

                    face_masks[ab_index + 2 * CHUNK_SIZE_2] = (column_bits & ~opaque_mask[a_cs_p + (b + 1)]) >> 1;
                    face_masks[ab_index + 3 * CHUNK_SIZE_2] = (column_bits & ~opaque_mask[a_cs_p + (b - 1)]) >> 1;

                    face_masks[ba_index + 4 * CHUNK_SIZE_2] = column_bits & ~(opaque_mask[a_cs_p + b] >> 1);
                    face_masks[ba_index + 5 * CHUNK_SIZE_2] = column_bits & ~(opaque_mask[a_cs_p + b] << 1);
                }
            }

            // Greedy meshing faces 0-3
            for (int face = 0; face < 4; face++)
            {
                const int axis = face / 2;

                const int face_vertex_begin = vertex_i;

                for (int layer = 0; layer < CHUNK_SIZE; layer++)
                {
                    const int bits_location = layer * CHUNK_SIZE + face * CHUNK_SIZE_2;

                    for (int forward = 0; forward < CHUNK_SIZE; forward++)
                    {
                        uint64_t bits_here = face_masks[forward + bits_location];
                        if (bits_here == 0)
                        {
                            continue;
                        }

                        const uint64_t bits_next = forward + 1 < CHUNK_SIZE ? face_masks[(forward + 1) + bits_location] : 0;

                        uint8_t right_merged_count = 1;
                        while (bits_here)
                        {
                            unsigned long bit_pos;
                            #ifdef _MSC_VER
                            _BitScanForward64(&bit_pos, bits_here);
                            #else
                            bit_pos = __builtin_ctzll(bits_here);
                            #endif

                            const uint8_t type = _voxels[get_axis_index(axis, forward + 1, bit_pos + 1, layer + 1)];
                            uint8_t& forward_merged_ref = forward_merged[bit_pos];

                            if ((bits_next >> bit_pos & 1) && type == _voxels[get_axis_index(axis, forward + 2, bit_pos + 1, layer + 1)])
                            {
                                forward_merged_ref++;
                                bits_here &= ~(1ull << bit_pos);
                                continue;
                            }

                            for (int right = bit_pos + 1; right < CHUNK_SIZE; right++)
                            {
                                if (!(bits_here >> right & 1) || forward_merged_ref != forward_merged[right] || type != _voxels[get_axis_index(
                                    axis,
                                    forward + 1,
                                    right + 1,
                                    layer + 1
                                )])
                                {
                                    break;
                                }
                                forward_merged[right] = 0;
                                right_merged_count++;
                            }
                            bits_here &= ~((1ull << (bit_pos + right_merged_count)) - 1);

                            const uint8_t mesh_front = forward - forward_merged_ref;
                            const uint8_t mesh_left = bit_pos;
                            const uint8_t mesh_up = layer + (~face & 1);

                            const uint8_t mesh_width = right_merged_count;
                            const uint8_t mesh_length = forward_merged_ref + 1;

                            forward_merged_ref = 0;
                            right_merged_count = 1;

                            uint64_t quad = 0;
                            switch (face)
                            {
                                case 0:
                                case 1:
                                {
                                    quad = get_quad(mesh_front + (face == 1 ? mesh_length : 0), mesh_up, mesh_left, mesh_length, mesh_width, type);
                                    break;
                                }
                                case 2:
                                case 3:
                                {
                                    quad = get_quad(mesh_up, mesh_front + (face == 2 ? mesh_length : 0), mesh_left, mesh_length, mesh_width, type);
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                            }

                            insert_quad(*_mesh_data.vertices, quad, vertex_i, _mesh_data.max_vertices);
                        }
                    }
                }

                const int face_vertex_length = vertex_i - face_vertex_begin;
                _mesh_data.face_vertex_begin[face] = face_vertex_begin;
                _mesh_data.face_vertex_length[face] = face_vertex_length;
            }

            // Greedy meshing faces 4-5
            for (int face = 4; face < 6; face++)
            {
                const int axis = face / 2;

                const int face_vertex_begin = vertex_i;

                for (int forward = 0; forward < CHUNK_SIZE; forward++)
                {
                    const int bits_location = forward * CHUNK_SIZE + face * CHUNK_SIZE_2;
                    const int bits_forward_location = (forward + 1) * CHUNK_SIZE + face * CHUNK_SIZE_2;

                    for (int right = 0; right < CHUNK_SIZE; right++)
                    {
                        uint64_t bits_here = face_masks[right + bits_location];
                        if (bits_here == 0)
                        {
                            continue;
                        }

                        const uint64_t bits_forward = forward < CHUNK_SIZE - 1 ? face_masks[right + bits_forward_location] : 0;
                        const uint64_t bits_right = right < CHUNK_SIZE - 1 ? face_masks[right + 1 + bits_location] : 0;
                        const int right_cs = right * CHUNK_SIZE;

                        while (bits_here)
                        {
                            unsigned long bit_pos;
                            #ifdef _MSC_VER
                            _BitScanForward64(&bit_pos, bits_here);
                            #else
                            bit_pos = __builtin_ctzll(bits_here);
                            #endif

                            bits_here &= ~(1ull << bit_pos);

                            const uint8_t type = _voxels[get_axis_index(axis, right + 1, forward + 1, bit_pos)];
                            uint8_t& forward_merged_ref = forward_merged[right_cs + (bit_pos - 1)];
                            uint8_t& right_merged_ref = right_merged[bit_pos - 1];

                            if (right_merged_ref == 0 && (bits_forward >> bit_pos & 1) && type == _voxels[get_axis_index(
                                axis,
                                right + 1,
                                forward + 2,
                                bit_pos
                            )])
                            {
                                forward_merged_ref++;
                                continue;
                            }

                            if ((bits_right >> bit_pos & 1) && forward_merged_ref == forward_merged[(right_cs + CHUNK_SIZE) + (bit_pos - 1)] && type == _voxels[
                                get_axis_index(
                                    axis,
                                    right + 2,
                                    forward + 1,
                                    bit_pos
                                )])
                            {
                                forward_merged_ref = 0;
                                right_merged_ref++;
                                continue;
                            }

                            const uint8_t mesh_left = right - right_merged_ref;
                            const uint8_t mesh_front = forward - forward_merged_ref;
                            const uint8_t mesh_up = bit_pos - 1 + (~face & 1);

                            const uint8_t mesh_width = 1 + right_merged_ref;
                            const uint8_t mesh_length = 1 + forward_merged_ref;

                            forward_merged_ref = 0;
                            right_merged_ref = 0;

                            const uint64_t quad = get_quad(mesh_left + (face == 4 ? mesh_width : 0), mesh_front, mesh_up, mesh_width, mesh_length, type);

                            insert_quad(*_mesh_data.vertices, quad, vertex_i, _mesh_data.max_vertices);
                        }
                    }
                }

                const int face_vertex_length = vertex_i - face_vertex_begin;
                _mesh_data.face_vertex_begin[face] = face_vertex_begin;
                _mesh_data.face_vertex_length[face] = face_vertex_length;
            }

            _mesh_data.vertex_count = vertex_i + 1;
        }
    };
}
