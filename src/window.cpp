#include "window.h"


VoxelEngine::Window::Window(u16 p_Width, u16 p_Height, const char* p_Title, bool p_FullScreen)
{
    VE_assert(p_Width != 0, "Screen width cannot be zero.");
    VE_assert(p_Height != 0, "Screen height cannot be zero.");
    VE_assert(p_Title != nullptr, "Title cannot be a nullptr");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_LastFrame = glfwGetTime();
    m_Window = glfwCreateWindow(p_Width, p_Height, p_Title, nullptr, nullptr);
    VE_assert(m_Window != nullptr, "Failed to create GLFW window.");

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(0);

    VE_assert(gladLoadGL(), "GLAD failed to load.");

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(VoxelEngine::debug_message_callback, nullptr);

    glViewport(0, 0, p_Width, p_Height);
}

VoxelEngine::Window::~Window()
{
    glfwTerminate();
}

bool VoxelEngine::Window::shouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

void VoxelEngine::Window::clear(u8 p_R, u8 p_G, u8 p_B, f32 p_A)
{
    f64 current_frame = glfwGetTime();
    m_DeltaTime = current_frame - m_LastFrame;
    m_LastFrame = current_frame;

    glClearColor(p_R / 255.0f, p_G / 255.0f, p_B / 255.0f, p_A);
    glClear(GL_COLOR_BUFFER_BIT);
}

void VoxelEngine::Window::swap()
{
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

f64 VoxelEngine::Window::getDeltaTime() const
{
    return m_DeltaTime;
}

f64 VoxelEngine::Window::getFps() const
{
    return 1.0f / m_DeltaTime;
}
