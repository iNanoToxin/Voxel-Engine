#include "debug.h"

std::vector<GLuint> ignored_ids = {131185};

namespace voxel_engine
{
    const char* get_source(const GLenum _source)
    {
        switch (_source)
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

    const char* get_type(const GLenum _type)
    {
        switch (_type)
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

    const char* get_severity(const GLenum _type)
    {
        switch (_type)
        {
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            default: return "UNKNOWN";
        }
    }

    void debug_message_callback(
        const GLenum _source,
        const GLenum _type,
        const GLuint _id,
        const GLenum _severity,
        const GLsizei _length,
        const char* _message,
        const void* _user_param
    )
    {
        (void) _length;
        (void) _user_param;

        for (const GLuint id : ignored_ids)
        {
            if (id == _id)
            {
                return;
            }
        }

        std::cerr << "Debug Message: " << _message << "\n"
            << "    " << "Source: " << get_source(_source) << "\n"
            << "    " << "Type: " << get_type(_type) << "\n"
            << "    " << "Id: " << _id << "\n"
            << "    " << "Severity: " << get_severity(_severity) << "\n";
        // std::exit(0);
    }
}
