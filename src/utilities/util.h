#ifndef VOXEL_ENGINE_UTIL_H
#define VOXEL_ENGINE_UTIL_H

#include <string>
#include <fstream>
#include <sstream>
#include "utilities/assert.h"

namespace VoxelEngine::Util
{
    std::string read_file(const char* p_FilePath);
    void write_file(const char* p_FilePath, const std::string& p_Source);
    u32 load_texture(char const* p_FilePath);
}


#endif
