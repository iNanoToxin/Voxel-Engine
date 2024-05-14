#include "window.h"

void framebuffer_size_callback(GLFWwindow* p_Window, i32 p_Width, i32 p_Height)
{
    glViewport(0, 0, p_Width, p_Height);
}

VoxelEngine::Window::Window(u16 p_Width, u16 p_Height, const char* p_Title, bool p_FullScreen)
{
    VE_assert(p_Width != 0, "Screen width cannot be zero.");
    VE_assert(p_Height != 0, "Screen height cannot be zero.");
    VE_assert(p_Title != nullptr, "Title cannot be a nullptr.");

    width = p_Width;
    height = p_Height;

    VE_assert(glfwInit(), "Failed to initialize GLFW library.");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 64);

    m_LastFrame = glfwGetTime();
    m_Window = glfwCreateWindow(p_Width, p_Height, p_Title, nullptr, nullptr);
    VE_assert(m_Window != nullptr, "Failed to create GLFW window.");

    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, &framebuffer_size_callback);
    glfwSwapInterval(0);

    VE_assert(gladLoadGL(), "Failed to initialize GLAD library.");

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(VoxelEngine::debug_message_callback, nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, p_Width, p_Height);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (p_FullScreen)
    {
        glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        glfwSetWindowPos(m_Window, (mode->width - width) / 2, (mode->height - height) / 2);
    }
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
