#include "./assert.h"
#include <sstream>
#include <iostream>

void voxel_engine::assert_condition(
    const int32_t _line,
    const char* _file,
    const char* _condition,
    const bool _success,
    const char* _message,
    const char* _identifier
)
{
    if (_success) return;

    std::stringstream stream;
    stream << "Assertion Failed: `" << _condition << "`" << "\n";
    if (_identifier != nullptr)
    {
        stream << "    " << "Identifier: " << _identifier << "\n";
    }
    stream << "    " << "Line: " << _line << "\n";
    stream << "    " << "File: " << _file << "\n";
    stream << "    " << "Message: " << _message;

    std::cerr << stream.rdbuf() << std::endl;
    std::exit(0);
}

void voxel_engine::failure(
    const int32_t _line,
    const char* _file,
    const char* _condition,
    const char* _message,
    const char* _identifier
)
{
    std::stringstream stream;
    stream << "Failure: " << _condition << "\n";
    if (_identifier != nullptr)
    {
        stream << "    " << "Identifier: " << _identifier << "\n";
    }
    stream << "    " << "Line: " << _line << "\n";
    stream << "    " << "File: " << _file << "\n";
    stream << "    " << "Message: " << _message;

    std::cerr << stream.rdbuf() << std::endl;
    std::exit(0);
}
