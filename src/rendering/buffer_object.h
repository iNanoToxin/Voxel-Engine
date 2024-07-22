#ifndef VOXEL_ENGINE_BUFFER_OBJECT_H
#define VOXEL_ENGINE_BUFFER_OBJECT_H

#include "glad/glad.h"
#include "utilities/types.h"
#include "vao.h"
#include "glm/gtc/type_ptr.hpp"

namespace VoxelEngine
{
    class Vao;

    class BufferObject
    {
    private:
        GLenum m_BufferType;
    protected:
        u32 m_Id{};
    public:
        explicit BufferObject(GLenum p_BufferType);

        virtual void bind() const;
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

    /*class Ubo : public BufferObject
    {
    public:
        Ubo() : BufferObject(GL_UNIFORM_BUFFER) {}

        void bind() const override
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_Id);
        }
    };*/

    class UniformBufferObject
    {
    private:
        u32 m_Id{};
        u32 m_Offset = 0;
    public:
        UniformBufferObject()
        {
            glGenBuffers(1, &m_Id);
        }

        ~UniformBufferObject()
        {
            glDeleteBuffers(1, &m_Id);
        }

        void allocate(u32 p_Size, GLenum p_Usage = GL_STATIC_DRAW) const
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
            glBufferData(GL_UNIFORM_BUFFER, p_Size, nullptr, p_Usage);
        }

        void bind(u32 p_BindingPoint) const
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, p_BindingPoint, m_Id);
        }

        void offset(u32 p_Offset)
        {
            m_Offset = p_Offset;
        }

        void update(u32 p_Size, const void* p_Data)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
            glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, p_Size, p_Data);
            m_Offset += p_Size;
        }

        template <typename T>
        void update(const T& p_Data)
        {
            update(sizeof(p_Data), glm::value_ptr(p_Data));
        }
    };
}

#endif
