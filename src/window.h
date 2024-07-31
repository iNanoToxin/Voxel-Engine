#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "common/types.h"
#include "utilities/assert.h"
#include "utilities/debug.h"

namespace voxel_engine
{
    class window
    {
    private:
        GLFWwindow* _window = nullptr;
        float64_t _delta_time = 0.0f;
        float64_t _last_frame;
    public:
        uint16_t width;
        uint16_t height;

        window(uint16_t _width, uint16_t _height, const char* _title, bool _full_screen = false);
        ~window();

        void clear(uint8_t _r, uint8_t _g, uint8_t _b, float32_t _a = 0.0f);
        void swap();

        [[nodiscard]] bool should_close();
        [[nodiscard]] float64_t get_delta_time() const;
        [[nodiscard]] float64_t get_fps() const;
        [[nodiscard]] GLFWwindow* get_window() const;
    };
}
