#pragma once
#include <string>
#include "utilities/assert.h"

namespace voxel_engine::util
{
    std::string read_file(const char* _file_path);
    void write_file(const char* _file_path, const std::string& _source);
    uint32_t load_texture(char const* _file_path);
}
