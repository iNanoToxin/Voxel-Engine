#pragma once
#include <camera.h>

#include "buffer_object.h"
#include "shader.h"

#define GET_SHADER(PATH) SHADERS_PATH PATH

namespace VoxelEngine
{
    class GridOverlay
    {
    private:
        VoxelEngine::Shader shader;
        VoxelEngine::Vao vao;
        VoxelEngine::Vbo vbo;
        VoxelEngine::Ebo ebo;
        VoxelEngine::UniformBufferObject ubo;
    public:
        explicit GridOverlay() : shader(GET_SHADER("grid.vert"), GET_SHADER("grid.frag"))
        {
            // @formatter:off
            constexpr f32 vertices[] = {
                -1.0f, -1.0f, 0.0f,
                 1.0f, -1.0f, 0.0f,
                 1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f
            };

            const u32 indices[] = {
                0, 1, 2,
                2, 3, 0
            };
            // @formatter:on

            vbo.data(vao, sizeof(vertices), vertices);
            ebo.data(vao, sizeof(indices), indices);

            vao.link(vbo, 3 * sizeof(float));
            vao.addAttribute(3, GL_FLOAT);

            ubo.allocate(3 * sizeof(glm::mat4) + sizeof(glm::vec3) + 2 * sizeof(float));
            ubo.bind(0);
        }

        void draw(const VoxelEngine::Camera& camera)
        {
            const float near = camera.getNear();
            const float far = camera.getFar();

            glCullFace(camera.getPosition().y >= 0 ? GL_FRONT : GL_BACK);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            shader.use();
            ubo.offset(0);
            ubo.update(camera.getViewMatrix());
            ubo.update(camera.getInverseViewMatrix());
            ubo.update(camera.getProjectionMatrix());
            ubo.update(camera.getPosition());
            ubo.update(sizeof(float), &near);
            ubo.update(sizeof(float), &far);
            ubo.bind(0);
            vao.bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glCullFace(GL_BACK);
        }
    };
}
