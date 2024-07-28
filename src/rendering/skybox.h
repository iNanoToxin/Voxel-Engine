#pragma once
#include <array>
#include <string>

#include "camera.h"
#include "shader.h"
#include "gl/cube_map.h"
#include "gl/vertex_array.h"

namespace voxel_engine
{
    class skybox
    {
    private:
        vertex_array _vao;
        cube_map _cube_map;
        shader _shader;
    public:
        explicit skybox(const std::array<std::string, 6>& _faces);

        void render(const camera& _camera) const;
    };
}
