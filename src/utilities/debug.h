#pragma once
#include "utilities/types.h"
#include "glad/glad.h"
#include <iostream>
#include <vector>

namespace voxel_engine
{
    static const char* get_source(GLenum _source);
    static const char* get_type(GLenum _type);
    static const char* get_severity(GLenum _type);

    void APIENTRY debug_message_callback(
        GLenum _source,
        GLenum _type,
        GLuint _id,
        GLenum _severity,
        GLsizei _length,
        const char* _message,
        const void* _user_param
    );
}
