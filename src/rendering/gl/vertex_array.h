#pragma once
#include <glad/glad.h>
#include "common/types.h"

namespace voxel_engine
{
    class vertex_array
    {
    private:
        uint32_t _id;
        uint32_t _stride = 0;
        uint32_t _offset = 0;

    public:
        vertex_array();
        ~vertex_array();

        uint32_t get_id() const;

        void set_offset(uint32_t _offset);
        void set_stride(uint32_t _stride);

        void bind_vertex_array() const;
        void add_attribute_int32(uint32_t _index, uint32_t _size, GLenum _type, uint32_t _data_size);
        void add_attribute_float32(uint32_t _index, uint32_t _size, GLenum _type, GLboolean _normalized, uint32_t _data_size);
        void add_attribute_float64(uint32_t _index, uint32_t _size, GLenum _type, uint32_t _data_size);

        void update_attribute_per_instance(uint32_t _index, uint32_t _instances) const;
        void draw_arrays_instanced(GLenum _mode, uint32_t _first, uint32_t _count, uint32_t _instances) const;
        void draw_arrays(GLenum _mode, uint32_t _first, uint32_t _count) const;
    };
}
