#include "vertex_buffer.h"

voxel_engine::vertex_buffer::vertex_buffer(const GLenum _buffer_type)
{
    glGenBuffers(1, &_id);
    this->_buffer_type = _buffer_type;
}

voxel_engine::vertex_buffer::~vertex_buffer()
{
    glDeleteBuffers(1, &_id);
}

uint32_t voxel_engine::vertex_buffer::get_id() const
{
    return _id;
}

void voxel_engine::vertex_buffer::set_offset(const uint32_t _offset)
{
    this->_offset = _offset;
}

void voxel_engine::vertex_buffer::bind_vertex_buffer() const
{
    glBindBuffer(_buffer_type, _id);
}

void voxel_engine::vertex_buffer::bind_vertex_buffer_base(const uint32_t _binding_point) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, _binding_point, _id);
}

void voxel_engine::vertex_buffer::set_buffer_data(const uint64_t _size, const void* _data, const GLenum _usage) const
{
    bind_vertex_buffer();
    glBufferData(_buffer_type, _size, _data, _usage);
}
