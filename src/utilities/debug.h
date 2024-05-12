#ifndef VOXELENGINE_DEBUG_H
#define VOXELENGINE_DEBUG_H

#include "utilities/types.h"
#include "glad/glad.h"
#include <iostream>

namespace VoxelEngine
{
    static const char* get_source(GLenum p_Source);
    static const char* get_type(GLenum p_Type);
    static const char* get_severity(GLenum p_Type);

    void APIENTRY debug_message_callback(GLenum p_Source, GLenum p_Type, GLuint p_Id, GLenum p_Severity, GLsizei p_Length, const char* p_Message, const void* p_UserParam);
}

#endif
