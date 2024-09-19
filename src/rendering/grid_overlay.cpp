#include "grid_overlay.h"
#include <common/constants.h>
#include <glm/gtc/type_ptr.hpp>

voxel_engine::grid_overlay::grid_overlay()
    : _shader("grid/grid.vert", "grid/grid.frag")
{
    glCreateVertexArrays(1, &_vao);
    glCreateBuffers(1, &_vbo);
    glCreateBuffers(1, &_ubo);

    // @formatter:off
    constexpr float32_t vertices[12] = {
       -1.0f, -1.0f, // bottom-left
        1.0f, -1.0f, // bottom-right
        1.0f,  1.0f, // top-right
        1.0f,  1.0f, // top-right
       -1.0f,  1.0f, // top-left
       -1.0f, -1.0f  // bottom-left
    };
    // @formatter:on

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);

    glNamedBufferData(_vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(
        _ubo,
        3 * sizeof(glm::mat4) + sizeof(glm::vec3) + 2 * sizeof(float32_t),
        nullptr,
        GL_STATIC_DRAW
    );
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo);

    glEnableVertexArrayAttrib(_vao, 0);
    glVertexArrayAttribFormat(_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_vao, 0, 0);
    glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, 2 * sizeof(float));
}

voxel_engine::grid_overlay::~grid_overlay()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ubo);
}

void voxel_engine::grid_overlay::render(const camera& _camera)
{
    glCullFace(_camera.position.y >= 0 ? GL_FRONT : GL_BACK);

    _shader.use();

    int32_t offset = 0;
    glNamedBufferSubData(_ubo, offset, sizeof(glm::mat4), glm::value_ptr(_camera.get_view_matrix()));
    glNamedBufferSubData(_ubo, offset += sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_camera.get_inverse_view_matrix()));
    glNamedBufferSubData(_ubo, offset += sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_camera.get_projection_matrix()));
    glNamedBufferSubData(_ubo, offset += sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(_camera.position));
    glNamedBufferSubData(_ubo, offset += sizeof(glm::vec3), sizeof(float32_t), &_camera.near);
    glNamedBufferSubData(_ubo, offset, sizeof(float32_t), &_camera.far);

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glCullFace(GL_BACK);
}
