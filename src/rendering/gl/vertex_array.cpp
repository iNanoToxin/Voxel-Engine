#include "vertex_array.h"

voxel_engine::vertex_array::vertex_array()
{
    glGenVertexArrays(1, &_id);
}

voxel_engine::vertex_array::~vertex_array()
{
    glDeleteVertexArrays(1, &_id);
}

uint32_t voxel_engine::vertex_array::get_id() const
{
    return _id;
}

void voxel_engine::vertex_array::set_offset(const uint32_t _offset)
{
    this->_offset = _offset;
}

void voxel_engine::vertex_array::set_stride(const uint32_t _stride)
{
    this->_stride = _stride;
}

void voxel_engine::vertex_array::bind_vertex_array() const
{
    glBindVertexArray(_id);
}

void voxel_engine::vertex_array::add_attribute_int32(
    const uint32_t _index,
    const uint32_t _size,
    const GLenum _type,
    const uint32_t _data_size
)
{
    bind_vertex_array();
    glEnableVertexAttribArray(_index);
    glVertexAttribIPointer(_index, _size, _type, _stride, reinterpret_cast<void*>(_offset));
    _offset += _size * _data_size;
}


void voxel_engine::vertex_array::add_attribute_float32(
    const uint32_t _index,
    const uint32_t _size,
    const GLenum _type,
    const GLboolean _normalized,
    const uint32_t _data_size
)
{
    bind_vertex_array();
    glEnableVertexAttribArray(_index);
    glVertexAttribPointer(_index, _size, _type, _normalized, _stride, reinterpret_cast<void*>(_offset));
    _offset += _size * _data_size;
}

void voxel_engine::vertex_array::add_attribute_float64(
    const uint32_t _index,
    const uint32_t _size,
    const GLenum _type,
    const uint32_t _data_size
)
{
    bind_vertex_array();
    glEnableVertexAttribArray(_index);
    glVertexAttribLPointer(_index, _size, _type, _stride, reinterpret_cast<void*>(_offset));
    _offset += _size * _data_size;
}

void voxel_engine::vertex_array::update_attribute_per_instance(const uint32_t _index, const uint32_t _instances) const
{
    bind_vertex_array();
    // updates the attribute every N instances rather than every vertex
    glVertexAttribDivisor(_index, _instances);
}

void voxel_engine::vertex_array::draw_arrays_instanced(
    const GLenum _mode,
    const uint32_t _first,
    const uint32_t _count,
    const uint32_t _instances
) const
{
    bind_vertex_array();
    glDrawArraysInstanced(_mode, _first, _count, _instances);
}

void voxel_engine::vertex_array::draw_arrays(const GLenum _mode, const uint32_t _first, const uint32_t _count) const
{
    bind_vertex_array();
    glDrawArrays(_mode, _first, _count);
}
