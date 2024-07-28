#pragma once
#include "types.h"

#define VE_assert(p_Success, ...) voxel_engine::assert_condition(__LINE__, __FILE__, #p_Success, p_Success, __VA_ARGS__)
#define VE_failure(...) voxel_engine::failure(__LINE__, __FILE__, __VA_ARGS__)

namespace voxel_engine
{
    void assert_condition(
        int32_t _line,
        const char* _file,
        const char* _condition,
        bool _success,
        const char* _message,
        const char* _identifier = nullptr
    );

    void failure(
        int32_t _line,
        const char* _file,
        const char* _condition,
        const char* _message,
        const char* _identifier = nullptr
    );
}
