#pragma once
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "texture.h"

namespace voxel_engine
{
    class cube_map_array : public texture
    {
    public:
        cube_map_array();

        void load_cube_map_array(const std::vector<std::array<std::string, 6>>& _textures) const;
    };
}
