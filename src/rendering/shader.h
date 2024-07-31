#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "common/types.h"


namespace voxel_engine
{
    class shader
    {
    private:
        uint32_t _id;

        [[nodiscard]] uint32_t get_uniform_location(const char* _uniform) const;
        [[nodiscard]] static uint32_t compile_shader(const char* _path, GLenum _type);
        [[nodiscard]] uint32_t link_shader(uint32_t _vertex_shader, uint32_t _fragment_shader) const;
    public:
        shader(const char* _vertex_shader, const char* _fragment_shader);

        void use() const;
        void set_mat4(const char* _uniform, glm::mat4 _mat4) const;
        void set_vec3(const char* _uniform, glm::vec3 _vec3) const;
        void set_float32(const char* _uniform, float32_t _float) const;
        void set_int32(const char* _uniform, int32_t _int) const;
    };
}
