#ifndef VOXEL_ENGINE_VAO_H
#define VOXEL_ENGINE_VAO_H

#include "glad/glad.h"
#include "utilities/types.h"

#include "buffer_object.h"

namespace VoxelEngine
{
    class BufferObject;

    class Vao
    {
    private:
        u32 m_Id{};
        u32 m_AttributeIndex = 0;
        u32 m_Stride = 0;
        u32 m_Offset = 0;
    public:
        Vao();

        void bind() const;
        void addAttribute(u32 p_Size, GLenum p_Type);
        void enableAttribute() const;
        void link(const BufferObject& p_Vbo, u32 p_Stride);
    };
}

#endif
