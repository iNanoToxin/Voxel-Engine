#pragma once

#define CHECK_VERTEX_ARRAY_BINDING(VERTEX_ARRAY)            \
do                                                          \
{                                                           \
    GLint bound_array;                                      \
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bound_array);   \
    assert(bound_array != 0);                               \
    assert(bound_array == VERTEX_ARRAY.get_id());           \
}                                                           \
while (false)

#define CHECK_VERTEX_ARRAY_UNBOUND(VERTEX_ARRAY)            \
do                                                          \
{                                                           \
    GLint bound_array;                                      \
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bound_array);   \
    assert(bound_array == 0);                               \
}                                                           \
while (false)

#define GL_CHECK_ERROR() \
do                                                          \
{                                                           \
    GLenum error = glGetError();                            \
    assert(error == GL_NO_ERROR);                           \
}                                                           \
while (false)


inline GLuint get_binding(const GLenum _name)
{
    switch (_name)
    {
        case GL_VERTEX_ARRAY: return GL_VERTEX_ARRAY_BINDING;

        case GL_ARRAY_BUFFER: return GL_ARRAY_BUFFER_BINDING;
        case GL_ATOMIC_COUNTER_BUFFER: return GL_ATOMIC_COUNTER_BUFFER_BINDING;
        case GL_COPY_READ_BUFFER: return GL_COPY_READ_BUFFER;
        case GL_COPY_WRITE_BUFFER: return GL_COPY_WRITE_BUFFER;
        case GL_DISPATCH_INDIRECT_BUFFER: return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
        case GL_DRAW_INDIRECT_BUFFER: return GL_DRAW_INDIRECT_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_PIXEL_PACK_BUFFER: return GL_PIXEL_PACK_BUFFER_BINDING;
        case GL_PIXEL_UNPACK_BUFFER: return GL_PIXEL_UNPACK_BUFFER_BINDING;
        case GL_SHADER_STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER_BINDING;
        case GL_TEXTURE_BUFFER: return GL_TEXTURE_BINDING_BUFFER;
        case GL_TRANSFORM_FEEDBACK_BUFFER: return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
        case GL_UNIFORM_BUFFER: return GL_UNIFORM_BUFFER_BINDING;

        default: return GL_NONE;
    }
}
