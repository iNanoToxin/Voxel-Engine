#include <iostream>
#include "shader.h"
#include "utilities/util.h"
#include "glm/gtc/type_ptr.hpp"

namespace voxel_engine
{
    shader::shader(const char* _vertex_shader, const char* _fragment_shader)
    {
        const uint32_t vertex_shader = compile_shader(_vertex_shader, GL_VERTEX_SHADER);
        const uint32_t fragment_shader = compile_shader(_fragment_shader, GL_FRAGMENT_SHADER);

        _id = link_shader(vertex_shader, fragment_shader);
    }

    uint32_t shader::compile_shader(const char* _path, const GLenum _type)
    {
        const std::string source = util::read_file(_path);
        const char* code = source.c_str();


        const uint32_t shader = glCreateShader(_type);
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        int32_t is_compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

        if (is_compiled == GL_FALSE)
        {
            int32_t max_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

            char info_log[max_length];
            glGetShaderInfoLog(shader, max_length, nullptr, info_log);
            VE_failure("Failed to compile shader.", info_log, _path);
        }
        return shader;
    }

    uint32_t shader::link_shader(const uint32_t _vertex_shader, const uint32_t _fragment_shader) const
    {

        const uint32_t program = glCreateProgram();
        glAttachShader(program, _vertex_shader);
        glAttachShader(program, _fragment_shader);
        glLinkProgram(program);

        int32_t is_linked;
        glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

        if (is_linked == GL_FALSE)
        {
            int32_t max_length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

            char info_log[max_length];
            glGetProgramInfoLog(program, max_length, nullptr, info_log);
            VE_failure("Failed to link shader.", info_log, std::to_string(_id).c_str());
        }

        glDetachShader(program, _vertex_shader);
        glDetachShader(program, _fragment_shader);
        glDeleteShader(_vertex_shader);
        glDeleteShader(_fragment_shader);
        return program;
    }

    void shader::use() const
    {
        glUseProgram(_id);
    }

    uint32_t shader::get_uniform_location(const char* _uniform) const
    {
        VE_assert(_uniform != nullptr, "Uniform name is nullptr.");
        const int32_t location = glGetUniformLocation(_id, _uniform);
        // VE_assert(location >= 0, "Uniform location is non-existent.", _uniform);
        return location;
    }

    void shader::set_mat4(const char* _uniform, glm::mat4 _mat4) const
    {
        glUniformMatrix4fv(get_uniform_location(_uniform), 1, GL_FALSE, glm::value_ptr(_mat4));
    }

    void shader::set_vec3(const char* _uniform, glm::vec3 _vec3) const
    {
        glUniform3fv(get_uniform_location(_uniform), 1, glm::value_ptr(_vec3));
    }

    void shader::set_float32(const char* _uniform, const float32_t _float) const
    {
        glUniform1f(get_uniform_location(_uniform), _float);
    }

    void shader::set_int32(const char* _uniform, const int32_t _int) const
    {
        glUniform1i(get_uniform_location(_uniform), _int);
    }
}
