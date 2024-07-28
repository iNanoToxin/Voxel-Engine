#include "grid_overlay.h"
#include <common/constants.h>
#include <glm/gtc/type_ptr.hpp>

voxel_engine::grid_overlay::grid_overlay()
    : _shader(GET_SHADER("grid/grid.vert"), GET_SHADER("grid/grid.frag"))
    , _vertex_buffer(GL_ARRAY_BUFFER)
    , _uniform_buffer(GL_UNIFORM_BUFFER)
{
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

    _vertex_array.bind_vertex_array();
    _vertex_buffer.set_buffer_data(sizeof(vertices), vertices, GL_STATIC_DRAW);
    _vertex_array.add_attribute_float32(0, 2, GL_FLOAT, GL_FALSE, sizeof(float32_t));

    _uniform_buffer.set_buffer_data(
        3 * sizeof(glm::mat4) + sizeof(glm::vec3) + 2 * sizeof(float32_t),
        nullptr,
        GL_STATIC_DRAW
    );
    _uniform_buffer.bind_vertex_buffer_base(0);
}

void voxel_engine::grid_overlay::render(const camera& _camera)
{
    glCullFace(_camera.position.y >= 0 ? GL_FRONT : GL_BACK);
    _shader.use();
    _vertex_array.bind_vertex_array();
    _uniform_buffer.set_offset(0);
    _uniform_buffer.set_buffer_subdata(sizeof(glm::mat4), _camera.get_view_matrix());
    _uniform_buffer.set_buffer_subdata(sizeof(glm::mat4), _camera.get_inverse_view_matrix());
    _uniform_buffer.set_buffer_subdata(sizeof(glm::mat4), _camera.get_projection_matrix());
    _uniform_buffer.set_buffer_subdata(sizeof(glm::vec3), _camera.position);
    _uniform_buffer.set_buffer_subdata(sizeof(float32_t), &_camera.near);
    _uniform_buffer.set_buffer_subdata(sizeof(float32_t), &_camera.far);
    _vertex_array.draw_arrays(GL_TRIANGLES, 0, 6);
    glCullFace(GL_BACK);
}
