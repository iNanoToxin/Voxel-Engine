#include "texture.h"
#include "utilities/assert.h"

voxel_engine::texture_data::texture_data(const std::string& _path)
{
    data = stbi_load(_path.c_str(), &width, &height, &nr_channels, 0);
    VE_assert(data != nullptr, "Failed to load image data.", _path.c_str());
    format = texture::get_format(nr_channels);
}

voxel_engine::texture_data::~texture_data()
{
    stbi_image_free(data);
}

voxel_engine::texture::texture(const GLenum _texture_type)
{
    glGenTextures(1, &_id);
    this->_texture_type = _texture_type;
}

voxel_engine::texture::~texture()
{
    glDeleteTextures(1, &_id);
}

uint32_t voxel_engine::texture::get_id() const
{
    return _id;
}

void voxel_engine::texture::bind_texture() const
{
    glBindTexture(_texture_type, _id);
}

void voxel_engine::texture::set_texture_parameter(const GLenum _parameter, const int32_t _value) const
{
    bind_texture();
    glTexParameteri(_texture_type, _parameter, _value);
}

void voxel_engine::texture::set_texture_parameter(const GLenum _parameter, const int32_t* _value) const
{
    bind_texture();
    glTexParameteriv(_texture_type, _parameter, _value);
}

void voxel_engine::texture::set_texture_parameter(const GLenum _parameter, const float32_t _value) const
{
    bind_texture();
    glTexParameterf(_texture_type, _parameter, _value);
}

void voxel_engine::texture::set_texture_parameter(const GLenum _parameter, const float32_t* _value) const
{
    bind_texture();
    glTexParameterfv(_texture_type, _parameter, _value);
}

void voxel_engine::texture::set_active_texture(const GLenum _texture_slot) const
{
    bind_texture();
    glActiveTexture(_texture_slot);
}

GLenum voxel_engine::texture::get_format(const uint32_t _nr_channels)
{
    switch (_nr_channels)
    {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_INVALID_ENUM;
    }
}

bool voxel_engine::texture::compare(const texture_data& _texture1, const texture_data& _texture2)
{
    return _texture1.width == _texture2.width && _texture1.height == _texture2.height && _texture1.nr_channels == _texture2.nr_channels;
}
