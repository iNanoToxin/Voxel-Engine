#ifndef VOXEL_ENGINE_ASSERT_H
#define VOXEL_ENGINE_ASSERT_H

#include "types.h"

#define VE_assert(p_Success, ...) VoxelEngine::assert_condition(__LINE__, __FILE__, #p_Success, p_Success, __VA_ARGS__)
#define VE_failure(...) VoxelEngine::failure(__LINE__, __FILE__, __VA_ARGS__)

namespace VoxelEngine
{
    void assert_condition(i32 p_Line, const char* p_File, const char* p_Condition, bool p_Success, const char* p_Message, const char* p_Identifier = nullptr);
    void failure(i32 p_Line, const char* p_File, const char* p_Condition, const char* p_Message, const char* p_Identifier = nullptr);
}

#endif
