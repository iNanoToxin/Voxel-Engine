#pragma once
#include <stb_image.h>
#include <string>
#include <glad/glad.h>
#include "common/types.h"


namespace voxel_engine
{
    struct texture_data
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t nr_channels = 0;
        GLenum format = GL_INVALID_ENUM;
        uint8_t* data = nullptr;

        explicit texture_data() = default;
        explicit texture_data(const std::string& _path);
        ~texture_data();
    };

    class texture
    {
    private:
        uint32_t _id;
        GLenum _texture_type;

    public:
        explicit texture(GLenum _texture_type);
        ~texture();

        uint32_t get_id() const;
        void bind_texture() const;
        void set_texture_parameter(GLenum _parameter, int32_t _value) const;
        void set_texture_parameter(GLenum _parameter, const int32_t* _value) const;
        void set_texture_parameter(GLenum _parameter, float32_t _value) const;
        void set_texture_parameter(GLenum _parameter, const float32_t* _value) const;
        void set_active_texture(GLenum _texture_slot) const;

        static GLenum get_format(uint32_t _nr_channels);
        static bool compare(const texture_data& _texture1, const texture_data& _texture2);
    };
}
