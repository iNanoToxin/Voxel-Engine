#include "./assert.h"
#include <iostream>

void VoxelEngine::assert_condition(bool p_Success, const char* p_Message, i32 p_Line, const char* p_File, const char* p_Condition)
{
    if (p_Success) return;
    std::cerr << "Assertion Failed: `" << p_Condition << "`" << "\n"
    << "    " << "Line: " << p_Line << "\n"
    << "    " << "File: " << p_File << "\n"
    << "    " << "Message: " << p_Message << std::endl;
    std::exit(0);
}
