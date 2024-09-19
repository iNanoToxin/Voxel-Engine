#pragma once
#include "camera.h"
#include "shader.h"

namespace voxel_engine
{
    class grid_overlay
    {
    private:
        shader _shader;
        GLuint _vao;
        GLuint _vbo;
        GLuint _ubo;

    public:
        explicit grid_overlay();
        ~grid_overlay();

        void render(const camera& _camera);
    };
}
