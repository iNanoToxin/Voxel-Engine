#pragma once
#include <array>
#include <string>
#include <glad/glad.h>
#include "texture.h"
#include "utilities/assert.h"

namespace voxel_engine
{
    class cube_map : public texture
    {
    public:
        cube_map() : texture(GL_TEXTURE_CUBE_MAP) {}

        void load_cube_map(const std::array<std::string, 6>& _faces) const
        {
            VE_assert(_faces.size() == 6, "Expected 6 faces in cube map.");
            bind_texture();

            texture_data first_image;

            for (uint32_t f_idx = 0; f_idx < 6; f_idx++)
            {
                // load texture using stbi_load
                texture_data image(_faces[f_idx]);

                // std::cout << image.width << " " << image.height << " " << image.nr_channels << std::endl;

                if (f_idx == 0)
                {
                    // get width, height, and format to compare against other textures
                    first_image.width = image.width;
                    first_image.height = image.height;
                    first_image.nr_channels = image.nr_channels;
                }
                else
                {
                    // check all images are same size and format
                    VE_assert(texture::compare(first_image, image), "All textures be same width, height, and format.", _faces[f_idx].c_str());
                }
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f_idx, 0, image.format, image.width, image.height, 0, image.format, GL_UNSIGNED_BYTE, image.data);
            }
        }
    };
}
