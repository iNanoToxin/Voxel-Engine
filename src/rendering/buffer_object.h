#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include "glad/glad.h"
#include "utilities/types.h"
#include "vao.h"

namespace VoxelEngine
{
    class Vao;

    class BufferObject
    {
    private:
        u32 m_Id{};
        GLenum m_BufferType;
    public:
        explicit BufferObject(GLenum p_BufferType);

        void bind() const;
        void data(const Vao& p_Vao, u32 p_Size, const void* p_Data) const;
    };

    class Vbo : public BufferObject
    {
    public:
        Vbo() : BufferObject(GL_ARRAY_BUFFER) {}
    };

    class Ebo : public BufferObject
    {
    public:
        Ebo() : BufferObject(GL_ELEMENT_ARRAY_BUFFER) {}
    };
}

#endif
