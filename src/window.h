#ifndef VOXELENGINE_WINDOW_H
#define VOXELENGINE_WINDOW_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "utilities/types.h"
#include "utilities/assert.h"
#include "utilities/debug.h"

namespace VoxelEngine
{
    class Window
    {
    private:
        GLFWwindow* m_Window = nullptr;
        f64 m_DeltaTime = 0.0f;
        f64 m_LastFrame;
    public:
        u16 width;
        u16 height;

        Window(u16 p_Width, u16 p_Height, const char* p_Title, bool p_FullScreen = false);
        ~Window();

        void clear(u8 p_R, u8 p_G, u8 p_B, f32 p_A = 0.0f);
        void swap();

        [[nodiscard]] bool shouldClose();
        [[nodiscard]] f64 getDeltaTime() const;
        [[nodiscard]] f64 getFps() const;
        [[nodiscard]] GLFWwindow* getWindow()
        {
            return m_Window;
        }
    };
}
#endif
