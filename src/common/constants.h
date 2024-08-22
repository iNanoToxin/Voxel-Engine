#pragma once
#include "types.h"

#define GET_DATA(PATH) DATA_PATH PATH
#define GET_SHADER(PATH) (std::string(SHADERS_PATH) + PATH).c_str()
#define GET_TEXTURE(PATH) (std::string(TEXTURES_PATH) + PATH).c_str()

#define VOXEL_SIZE 1
#define HALF_VOXEL_SIZE (VOXEL_SIZE * 0.5)

#define CHUNK_SIZE 32
#define CHUNK_SIZE_2 (CHUNK_SIZE * CHUNK_SIZE)
#define CHUNK_SIZE_3 (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

constexpr int32_t CHUNK_SIZE_PADDED = CHUNK_SIZE + 2;
constexpr int32_t CHUNK_SIZE_PADDED_2 = CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED;
constexpr int32_t CHUNK_SIZE_PADDED_3 = CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED;

constexpr uint64_t PADDING_MASK = ~(1ull << (CHUNK_SIZE_PADDED - 1) | 1); // -> 0111111111111111111111111111111111111111111111111111111111111110
// constexpr uint64_t PADDING_MASK = 0b1111111111111111111111111111111111111111111111111111111111111111;

#define BENCHMARK(BODY)                                                         \
std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); \
BODY                                                                            \
std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();   \
std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[s]" << std::endl;
