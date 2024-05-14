#include "./assert.h"
#include <sstream>
#include <iostream>

void VoxelEngine::assert_condition(i32 p_Line, const char* p_File, const char* p_Condition, bool p_Success, const char* p_Message, const char* p_Identifier)
{
    if (p_Success) return;

    std::stringstream stream;
    stream << "Assertion Failed: `" << p_Condition << "`" << "\n";
    if (p_Identifier != nullptr)
    {
        stream << "    " << "Identifier: " << p_Identifier << "\n";
    }
    stream << "    " << "Line: " << p_Line << "\n";
    stream << "    " << "File: " << p_File << "\n";
    stream << "    " << "Message: " << p_Message;

    std::cerr << stream.rdbuf() << std::endl;
    std::exit(0);
}

void VoxelEngine::failure(i32 p_Line, const char* p_File, const char* p_Condition, const char* p_Message, const char* p_Identifier)
{
    std::stringstream stream;
    stream << "Failure: " << p_Condition << "\n";
    if (p_Identifier != nullptr)
    {
        stream << "    " << "Identifier: " << p_Identifier << "\n";
    }
    stream << "    " << "Line: " << p_Line << "\n";
    stream << "    " << "File: " << p_File << "\n";
    stream << "    " << "Message: " << p_Message;

    std::cerr << stream.rdbuf() << std::endl;
    std::exit(0);
}
