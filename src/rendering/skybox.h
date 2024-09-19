#pragma once
#include <array>
#include <string>

#include "camera.h"
#include "shader.h"
#include "gl/cube_map.h"

namespace voxel_engine
{
    class skybox
    {
    private:
        cube_map _cube_map;
        shader _shader;

        GLuint _vao;
    public:
        explicit skybox(const std::array<std::string, 6>& _faces);

        void render(const camera& _camera) const;
    };
}
