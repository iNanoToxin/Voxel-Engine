#pragma once
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "utilities/types.h"

namespace voxel_engine
{
    class vertex_buffer
    {
    private:
        uint32_t _id;
        uint32_t _offset = 0;
        GLenum _buffer_type;

    public:
        explicit vertex_buffer(GLenum _buffer_type);
        ~vertex_buffer();

        uint32_t get_id() const;

        void set_offset(uint32_t _offset);

        void bind_vertex_buffer() const;

        void bind_vertex_buffer_base(uint32_t _binding_point) const;

        void set_buffer_data(uint64_t _size, const void* _data, GLenum _usage) const;

        template<typename T>
        void set_buffer_subdata(const uint64_t _data_size, const T* _data)
        {
            bind_vertex_buffer();
            glBufferSubData(_buffer_type, _offset, _data_size, static_cast<const void*>(_data));
            _offset += _data_size;
        }

        template<typename T>
        void set_buffer_subdata(const uint64_t _data_size, const T& _data)
        {
            set_buffer_subdata(_data_size, glm::value_ptr(_data));
        }
    };
}
