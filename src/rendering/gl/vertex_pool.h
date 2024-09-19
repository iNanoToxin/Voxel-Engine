#pragma once
#include <algorithm>
#include <deque>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace voxel_engine
{
    struct vertex
    {
        float position[3];
        float normal[3];
        float color[4];

        vertex(const glm::vec3 _p, const glm::vec3 _n, const glm::vec3 _c)
        {
            position[0] = _p.x;
            position[1] = _p.y;
            position[2] = _p.z;
            normal[0] = _n.x;
            normal[1] = _n.y;
            normal[2] = _n.z;
            color[0] = _c.x;
            color[1] = _c.y;
            color[2] = _c.z;
            color[3] = 1.0;
        }

        static void format(const uint32_t _vbo)
        {
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 0);

            glVertexBindingDivisor(0, 0);
            glVertexBindingDivisor(1, 0);
            glVertexBindingDivisor(2, 0);

            glVertexAttribBinding(0, 0);
            glVertexAttribBinding(1, 1);
            glVertexAttribBinding(2, 2);

            glBindVertexBuffer(0, _vbo, offsetof(vertex, position), sizeof(vertex));
            glBindVertexBuffer(1, _vbo, offsetof(vertex, normal), sizeof(vertex));
            glBindVertexBuffer(2, _vbo, offsetof(vertex, color), sizeof(vertex));
        }
    };

    // draw arrays indirect command
    struct daic
    {
        // base properties
        uint32_t indices;       // total number of indices to draw
        uint32_t instances;     // total number of instances
        uint32_t start_index;   // first index to draw (Start pos in index buffer)
        uint32_t base_vertex;   // first vertex to draw (i.e. start pos in VBO)
        uint32_t base_instance; // not quite sure... not important for me lol (I think)

        uint32_t* index = nullptr;
        uint32_t group;
        glm::vec3 position{};

        daic() = default;

        daic(
            const uint32_t _indices,
            const uint32_t _instances,
            const uint32_t _start_index,
            const uint32_t _base_vertex,
            uint32_t* _index,
            const uint32_t _group
        )
        {
            indices = _indices;
            instances = _instances;
            start_index = _start_index;
            base_vertex = _base_vertex;
            base_instance = 0;
            index = _index;
            group = _group;
        }
    };

    template<typename T>
    class vertex_pool
    {
    private:
        GLuint _vertex_array;
        GLuint _vertex_buffer;
        GLuint _element_array_buffer;
        GLuint _indirect_draw_command_buffer;

        uint64_t _k_size = 0;           // number of vertices per bucket
        uint64_t _n_buckets = 0;        // number of max buckets
        uint64_t _m_active_buckets = 0; // number of active buckets
        uint64_t _max_size = 0;         // max bucket size

        T* _start = nullptr;
        std::deque<T*> _free;

        std::vector<daic> _indirect_draw_commands;
        std::vector<uint64_t> _indices;
        std::vector<uint64_t> _group_indirect;

        vertex_pool()
            : _vertex_buffer(GL_SHADER_STORAGE_BUFFER)
            , _element_array_buffer(GL_ELEMENT_ARRAY_BUFFER)
            , _indirect_draw_command_buffer(GL_DRAW_INDIRECT_BUFFER)
        {
            glCreateVertexArrays(1, &_vertex_array);
            glCreateBuffers(1, &_vertex_buffer);
            glCreateBuffers(1, &_element_array_buffer);
            glCreateBuffers(1, &_indirect_draw_command_buffer);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, _vertex_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_array_buffer);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_draw_command_buffer);

            T::format(_vertex_buffer);
        }

    public:
        vertex_pool(const uint64_t _k_size, const uint64_t _n_buckets)
            : vertex_pool()
        {
            reserve(_k_size, _n_buckets);
            index();
        }

        ~vertex_pool()
        {
            while (!_indirect_draw_commands.empty())
            {
                unsection(_indirect_draw_commands[0].index);
            }
            glUnmapBuffer(_vertex_buffer);
        }

        // allocate the persistently mapped buffer & create buckets for pool
        void reserve(const uint32_t _k_size, const uint32_t _n_buckets)
        {
            this->_k_size = _k_size;
            this->_n_buckets = _n_buckets;
            this->_m_active_buckets = _n_buckets;
            this->_max_size = _k_size * _n_buckets;

            constexpr GLbitfield flag = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

            glNamedBufferStorage(_vertex_buffer, _k_size * _n_buckets * sizeof(T), nullptr, flag);
            // _start = (T*) (glMapBufferRange(GL_ARRAY_BUFFER, 0, _k_size * _n_buckets * sizeof(T), flag));

            for (uint32_t i = 0; i < _n_buckets; i++)
            {
                _free.push_front(_start + i * _k_size);
            }
        }

        void clear()
        {
            while (!_indirect_draw_commands.empty())
            {
                unsection(_indirect_draw_commands.back().index);
            }

            while (!_free.empty())
            {
                _free.pop_back();
            }

            for (uint64_t i = 0; i < _n_buckets; i++)
            {
                _free.push_front(_start + i * _k_size);
            }

            update();
        }

        uint32_t* section(const int32_t _size, const uint32_t _group = 0, const glm::vec3 _position = glm::vec3(0.0))
        {
            if (_size == 0 || _size > _k_size)
            {
                std::cout << "Vertexpool Error: Insufficient Bucket Size" << std::endl;
                return nullptr;
            }
            if (_free.empty())
            {
                std::cout << "Vertexpool Error: No More Buckets Available" << std::endl;
                return nullptr;
            }

            constexpr uint32_t first = _group_indirect[_group]; // first position in index buffer
            const uint32_t base = _free.back() - _start;        // first vertex

            // add draw call
            _indirect_draw_commands.emplace_back(_size, 1, first, base, new uint32_t(_indirect_draw_commands.size()), _group);
            _free.pop_back(); // remove bucket from memory pool

            _indirect_draw_commands.back().position = _position;
            return _indirect_draw_commands.back().index; // return reference to daic
        }

        void unsection(const uint32_t* _index)
        {
            if (_index == nullptr)
            {
                std::cout << "Vertexpool Error: Can't Unsection - Index is NULL" << std::endl;
                return;
            }

            for (uint32_t k = _indirect_draw_commands[*_index].base_vertex; k < _k_size; k++)
            {
                // deconstruct vertices in vertex pool
                (_start + k)->~T();
            }
            // add bucket to free queue
            _free.push_front(_start + _indirect_draw_commands[*_index].base_vertex);
            // erase indirect draw call
            std::swap(_indirect_draw_commands[*_index], _indirect_draw_commands.back());
            _indirect_draw_commands.pop_back();
            *_indirect_draw_commands[*_index].index = *_index; // copy value
            delete _index;
        }

        template<typename ...Args>
        void fill(uint32_t* _index, const int32_t _k, Args&& ..._args)
        {
            // find location in vertex pool for this vertex
            T* place = _start + _indirect_draw_commands[*_index].base_vertex + _k;

            if (static_cast<uint64_t>(place - _start) > _max_size)
            {
                std::cout << "Vertexpool Error: Out-Of-Bounds Write" << std::endl;
            }

            // construct vertex in place by argument forwarding
            try
            {
                new(place) T(std::forward<Args>(_args) ...);
            }
            catch (...)
            {
                throw;
            }
        }

        template<typename F, typename ...Args>
        void mask(F _func, Args&& ..._args)
        {
            if (_indirect_draw_commands.empty())
            {
                return;
            }

            uint64_t m = 0;                                  // number of draw calls
            uint64_t j = _indirect_draw_commands.size() - 1; // backside approach

            // execute sorting pass
            while (m <= j)
            {
                while (_func(_indirect_draw_commands[m], _args ...) && m < j)
                {
                    m++;
                }
                while (!_func(_indirect_draw_commands[j], _args ...) && m < j)
                {
                    j--;
                }
                *_indirect_draw_commands[m].index = j;
                *_indirect_draw_commands[j].index = m;
                std::swap(_indirect_draw_commands[m++], _indirect_draw_commands[j--]);
            }
            _m_active_buckets = m;
        }

        template<typename F, typename ...Args>
        void order(F _func, Args&& ..._args)
        {
            if (_indirect_draw_commands.empty())
            {
                return;
            }

            std::sort(
                _indirect_draw_commands.begin() + _m_active_buckets,
                [&](const daic& _a, const daic& _b)
                {
                    return _func(_a, _b, _args ...);
                }
            );

            // set correct indices after sorting
            for (uint64_t i = 0; i < _indirect_draw_commands.size(); i++)
            {
                *_indirect_draw_commands[i].index = i;
            }
        }

        void resize(const uint32_t* _index, const uint32_t _new_size)
        {
            if (_index != nullptr && *_index < _n_buckets)
            {
                _indirect_draw_commands[*_index].indices = _new_size;
            }
        }

        void index()
        {
            for (uint64_t j = 0; j < _k_size; j++)
            {
                _indices.push_back(j * 4 + 0);
                _indices.push_back(j * 4 + 1);
                _indices.push_back(j * 4 + 2);
                _indices.push_back(j * 4 + 3);
                _indices.push_back(j * 4 + 1);
                _indices.push_back(j * 4 + 0);
            }

            for (uint64_t j = 0; j < _k_size; j++)
            {
                _indices.push_back(j * 4 + 0);
                _indices.push_back(j * 4 + 2);
                _indices.push_back(j * 4 + 1);
                _indices.push_back(j * 4 + 1);
                _indices.push_back(j * 4 + 3);
                _indices.push_back(j * 4 + 0);
            }

            _group_indirect.push_back(0);
            _group_indirect.push_back(6 * _k_size);
            _group_indirect.push_back(0);
            _group_indirect.push_back(6 * _k_size);
            _group_indirect.push_back(0);
            _group_indirect.push_back(6 * _k_size);

            glNamedBufferData(_element_array_buffer, _indices.size() * sizeof(uint32_t), _indices.data(), GL_STATIC_DRAW);
        }

        void update() const
        {
            glNamedBufferData(_indirect_draw_command_buffer, _indirect_draw_commands.size() * sizeof(daic), _indirect_draw_commands.data(), GL_DYNAMIC_DRAW);
        }

        void render(const GLenum _mode = GL_TRIANGLES, const uint64_t _first = 0, const uint64_t _length = 0) const
        {
            glBindVertexArray(_vertex_array);
            glMultiDrawElementsIndirect(_mode, GL_UNSIGNED_INT, reinterpret_cast<void*>(_first * (sizeof(daic))), _length, sizeof(daic));
        }
    };
}
