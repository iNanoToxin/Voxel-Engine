#include "buffer_object.h"

namespace VoxelEngine {
    BufferObject::BufferObject(GLenum p_BufferType)
    {
        m_BufferType = p_BufferType;
        glGenBuffers(1, &m_Id);
    }

    void BufferObject::bind() const
    {
        glBindBuffer(m_BufferType, m_Id);
    }

    void BufferObject::data(const Vao& p_Vao, u32 p_Size, const void* p_Data) const
    {
        p_Vao.bind();
        bind();
        glBufferData(m_BufferType, p_Size, p_Data, GL_STATIC_DRAW);
    }
}
