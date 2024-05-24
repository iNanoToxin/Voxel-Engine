#ifndef VOXELENGINE_SHADER_H
#define VOXELENGINE_SHADER_H

#include <string>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "utilities/types.h"
#include "utilities/util.h"
#include "glm/gtc/type_ptr.hpp"

namespace VoxelEngine
{
    class Shader
    {
    private:
        // u32 m_Id;

        u32 getUniformLocation(const char* p_Uniform) const;
    public:
        u32 m_Id;
        Shader(const char* p_VertexShader, const char* p_FragmentShader);

        [[nodiscard]] u32 compileShader(const char* p_Path, GLenum p_Type) const;
        [[nodiscard]] u32 linkShader(u32 p_VertexShader, u32 p_FragmentShader) const;

        void use() const;
        void setMat4(const char* p_Uniform, glm::mat4 p_Mat4);
        void setVec3(const char* p_Uniform, glm::vec3 p_Vec3)
        {
            glUniform3fv(getUniformLocation(p_Uniform), 1, glm::value_ptr(p_Vec3));
        }
        void setFloat(const char* p_Uniform, float p_Float)
        {
            glUniform1f(getUniformLocation(p_Uniform), p_Float);
        }
    };
}

#endif
