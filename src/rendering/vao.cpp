#include "vao.h"
#include "utilities/assert.h"

namespace VoxelEngine
{
    Vao::Vao()
    {
        glGenVertexArrays(1, &m_Id);
    }

    void Vao::bind() const
    {
        glBindVertexArray(m_Id);
    }

    void Vao::addAttribute(u32 p_Size, GLenum p_Type)
    {
        u32 enum_size = 0;
        switch (p_Type)
        {
            case GL_FLOAT:
            {
                enum_size = 4;
                break;
            }
            default:
            {
                VE_failure("p_Type != GLenum", "Enum is not supported.");
                break;
            }
        }

        bind();
        glVertexAttribPointer(m_AttributeIndex, p_Size, p_Type, GL_FALSE, m_Stride, (void*) m_Offset);
        enableAttribute();
        m_Offset += enum_size * p_Size;
        m_AttributeIndex++;
    }

    void Vao::enableAttribute() const
    {
        glEnableVertexAttribArray(m_AttributeIndex);
    }

    void Vao::link(const BufferObject& p_Vbo, u32 p_Stride)
    {
        m_Stride = p_Stride;
        m_Offset = 0;
    }
}
