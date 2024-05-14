#include "debug.h"

std::vector<GLuint> ignored_ids = {131185};

namespace VoxelEngine
{
    const char* get_source(GLenum p_Source)
    {
        switch (p_Source)
        {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "UNKNOWN";
        }
    }

    const char* get_type(GLenum p_Type)
    {
        switch (p_Type)
        {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_PUSH_GROUP: return "PUSH_GROUP";
            case GL_DEBUG_TYPE_POP_GROUP: return "POP_GROUP";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "UNKNOWN";
        }
    }

    const char* get_severity(GLenum p_Type)
    {
        switch (p_Type)
        {
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            default: return "UNKNOWN";
        }
    }

    void debug_message_callback(GLenum p_Source, GLenum p_Type, GLuint p_Id, GLenum p_Severity, GLsizei p_Length, const char* p_Message, const void* p_UserParam)
    {
        (void) p_Length;
        (void) p_UserParam;

        for (GLuint id : ignored_ids)
        {
            if (id == p_Id)
            {
                return;
            }
        }

        std::cerr << "Debug Message: " << p_Message << "\n"
            << "    " << "Source: " << get_source(p_Source) << "\n"
            << "    " << "Type: " << get_type(p_Type) << "\n"
            << "    " << "Id: " << p_Id << "\n"
            << "    " << "Severity: " << get_severity(p_Severity) << "\n";
        // std::exit(0);
    }
}
