#include "util.h"
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <glad/glad.h>

std::string voxel_engine::util::read_file(const char* _file_path)
{
    VE_assert(_file_path != nullptr, "File path is nullptr.");

    std::stringstream stream;

    try
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(_file_path);
        stream << file.rdbuf();
        file.close();
    }
    catch (std::ifstream::failure& err)
    {
        VE_failure("Failed to open file.", err.what(), _file_path);
    }
    return stream.str();
}

void voxel_engine::util::write_file(const char* _file_path, const std::string& _source)
{
    VE_assert(_file_path != nullptr, "File path is nullptr.");

    try
    {
        std::ofstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(_file_path);
        file << _source;
        file.close();
    }
    catch (std::ofstream::failure& err)
    {
        VE_failure("Failed to open file.", err.what(), _file_path);
    }
}

uint32_t voxel_engine::util::load_texture(char const* _file_path)
{
    uint32_t texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    int32_t width, height, nr_components;
    // stbi_set_flip_vertically_on_load(true);

    if (uint8_t* data = stbi_load(_file_path, &width, &height, &nr_components, 0))
    {
        GLenum format{};
        if (nr_components == 1)
        {
            format = GL_RED;
        }
        else if (nr_components == 3)
        {
            format = GL_RGB;
        }
        else if (nr_components == 4)
        {
            format = GL_RGBA;
        }


        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        stbi_image_free(data);
        VE_failure("Failed to load texture.", _file_path);
    }
    return texture_id;
}
