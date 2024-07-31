#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "texture.h"
#include "utilities/assert.h"

namespace voxel_engine
{
    class texture_2d_array : public texture
    {
    public:
        texture_2d_array() : texture(GL_TEXTURE_2D_ARRAY) {}

        void load_texture_2d_array(const std::vector<std::string>& _textures) const
        {
            VE_assert(!_textures.empty(), "Passed 0 textures to cube map array.");
            bind_texture();

            texture_data first_image;

            for (uint32_t t_idx = 0; t_idx < _textures.size(); t_idx++)
            {
                // load texture using stbi_load
                texture_data image(_textures[t_idx], true);

                // std::cout << image.width << " " << image.height << " " << image.nr_channels << std::endl;

                if (t_idx == 0)
                {
                    // get width, height, and format to compare against other textures
                    first_image.width = image.width;
                    first_image.height = image.height;
                    first_image.nr_channels = image.nr_channels;
                    // defines the texture's dimensions, format, and allocates the necessary memory without initializing it with actual image data
                    // glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, image.format, image.width, image.height, _textures.size());
                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, image.format, image.width, image.height, _textures.size(), 0, image.format, GL_UNSIGNED_BYTE, nullptr);
                }
                else
                {
                    // check all images are same size and format
                    VE_assert(texture::compare(first_image, image), "All textures be same width, height, and format.", _textures[t_idx].c_str());
                }
                // updates a portion of the texture with new data
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, t_idx, image.width, image.height, 1, image.format, GL_UNSIGNED_BYTE, image.data);
            }
        }
    };
}
