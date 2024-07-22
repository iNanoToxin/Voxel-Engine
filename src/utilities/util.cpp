#include "util.h"

#include <stb_image.h>
#include <glad/glad.h>

std::string VoxelEngine::Util::read_file(const char* p_FilePath)
{
    VE_assert(p_FilePath != nullptr, "File path is nullptr.");

    std::stringstream stream;

    try
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(p_FilePath);
        stream << file.rdbuf();
        file.close();
    }
    catch (std::ifstream::failure& err)
    {
        VE_failure("Failed to open file.", err.what(), p_FilePath);
    }
    return stream.str();
}

void VoxelEngine::Util::write_file(const char* p_FilePath, const std::string& p_Source)
{
    VE_assert(p_FilePath != nullptr, "File path is nullptr.");

    try
    {
        std::ofstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(p_FilePath);
        file << p_Source;
        file.close();
    }
    catch (std::ofstream::failure& err)
    {
        VE_failure("Failed to open file.", err.what(), p_FilePath);
    }
}

u32 VoxelEngine::Util::load_texture(char const* p_FilePath)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    if (unsigned char* data = stbi_load(p_FilePath, &width, &height, &nrComponents, 0))
    {
        GLenum format{};
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        stbi_image_free(data);
        VE_failure("Failed to load texture.", p_FilePath);
    }
    return textureID;
}
