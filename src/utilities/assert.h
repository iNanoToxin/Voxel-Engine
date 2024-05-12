#ifndef VOXELENGINE_ASSERT_H
#define VOXELENGINE_ASSERT_H

#include "types.h"

#define VE_assert(p_Success, p_Message) VoxelEngine::assert_condition(p_Success, p_Message, __LINE__, __FILE__, #p_Success)

namespace VoxelEngine
{
    void assert_condition(bool p_Success, const char* p_Message, i32 p_Line, const char* p_File, const char* p_Condition);
}

#endif
