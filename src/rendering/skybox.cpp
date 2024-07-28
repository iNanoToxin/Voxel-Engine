#include "skybox.h"
#include <stb_image.h>
#include "common/constants.h"

voxel_engine::skybox::skybox(const std::array<std::string, 6>& _faces)
    : _shader(GET_SHADER("skybox/skybox.vert"), GET_SHADER("skybox/skybox.frag"))
{
    _shader.use();
    _shader.set_int32("skybox", 0);
    _cube_map.load_cube_map(_faces);
    _cube_map.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _cube_map.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _cube_map.set_texture_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    _cube_map.set_texture_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    _cube_map.set_texture_parameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void voxel_engine::skybox::render(const camera& _camera) const
{
    glDepthFunc(GL_LEQUAL);
    _shader.use();
    _shader.set_mat4("view", glm::mat4(glm::mat3(_camera.get_view_matrix())));
    _shader.set_mat4("projection", _camera.get_projection_matrix());
    _cube_map.set_active_texture(GL_TEXTURE0);
    _vao.draw_arrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}
