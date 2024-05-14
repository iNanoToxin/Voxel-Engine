#include <iostream>
#include "shader.h"

namespace VoxelEngine
{
    Shader::Shader(const char* p_VertexShader, const char* p_FragmentShader)
    {
        u32 vertex_shader = compileShader(p_VertexShader, GL_VERTEX_SHADER);
        u32 fragment_shader = compileShader(p_FragmentShader, GL_FRAGMENT_SHADER);

        m_Id = linkShader(vertex_shader, fragment_shader);
    }

    u32 Shader::compileShader(const char* p_Path, GLenum p_Type) const
    {
        std::string source = Util::read_file(p_Path);
        const char* code = source.c_str();

        char info_log[512];
        i32 success;

        u32 shader = glCreateShader(p_Type);
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, info_log);
            VE_failure("Failed to compile shader.", info_log, p_Path);
        }
        return shader;
    }

    u32 Shader::linkShader(u32 p_VertexShader, u32 p_FragmentShader) const
    {
        int success;
        char info_log[512];

        u32 shader_id = glCreateProgram();
        glAttachShader(shader_id, p_VertexShader);
        glAttachShader(shader_id, p_FragmentShader);
        glLinkProgram(shader_id);

        glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader_id, 512, nullptr, info_log);
            VE_failure("Failed to link shader.", info_log, std::to_string(m_Id).c_str());
        }

        glDeleteShader(p_VertexShader);
        glDeleteShader(p_FragmentShader);
        return shader_id;
    }

    void Shader::use() const
    {
        glUseProgram(m_Id);
    }

    u32 Shader::getUniformLocation(const char* p_Uniform) const
    {
        VE_assert(p_Uniform != nullptr, "Uniform name is nullptr.");
        i32 location = glGetUniformLocation(m_Id, p_Uniform);
        VE_assert(location >= 0, "Uniform location is non-existant.", p_Uniform);
        return location;
    }

    void Shader::setMat4(const char* p_Uniform, glm::mat4 p_Mat4)
    {
        glUniformMatrix4fv(getUniformLocation(p_Uniform), 1, GL_FALSE, glm::value_ptr(p_Mat4));
    }
}
