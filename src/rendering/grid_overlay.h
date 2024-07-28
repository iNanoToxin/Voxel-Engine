#pragma once
#include "camera.h"
#include "shader.h"
#include "rendering/gl/vertex_array.h"
#include "rendering/gl/vertex_buffer.h"


namespace voxel_engine
{
    class grid_overlay
    {
    private:
        shader _shader;
        vertex_array _vertex_array;
        vertex_buffer _vertex_buffer;
        vertex_buffer _uniform_buffer;

    public:
        explicit grid_overlay();

        void render(const camera& _camera);
    };
}
