#include "window.h"

void framebuffer_size_callback(GLFWwindow* p_Window, int32_t p_Width, int32_t p_Height)
{
    glViewport(0, 0, p_Width, p_Height);
}

voxel_engine::window::window(const uint16_t _width, const uint16_t _height, const char* _title, const bool _full_screen)
{
    VE_assert(_width != 0, "Screen width cannot be zero.");
    VE_assert(_height != 0, "Screen height cannot be zero.");
    VE_assert(_title != nullptr, "Title cannot be a nullptr.");

    VE_assert(glfwInit(), "Failed to initialize GLFW library.");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 64);

    _last_frame = glfwGetTime();
    _window = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);
    VE_assert(_window != nullptr, "Failed to create GLFW window.");

    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, &framebuffer_size_callback);
    glfwSwapInterval(0);

    VE_assert(gladLoadGL(), "Failed to initialize GLAD library.");

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_message_callback, nullptr);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, _width, _height);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (_full_screen)
    {
        width = mode->width;
        height = mode->height;
        glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        width = _width;
        height = _height;
        glfwSetWindowPos(_window, (mode->width - width) / 2, (mode->height - height) / 2);
    }
}

voxel_engine::window::~window()
{
    glfwTerminate();
}

bool voxel_engine::window::should_close()
{
    return glfwWindowShouldClose(_window);
}

void voxel_engine::window::clear(const uint8_t _r, const uint8_t _g, const uint8_t _b, const float32_t _a)
{
    float64_t current_frame = glfwGetTime();
    _delta_time = current_frame - _last_frame;
    _last_frame = current_frame;

    glClearColor(_r / 255.0f, _g / 255.0f, _b / 255.0f, _a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void voxel_engine::window::swap()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

float64_t voxel_engine::window::get_delta_time() const
{
    return _delta_time;
}

float64_t voxel_engine::window::get_fps() const
{
    return 1.0f / _delta_time;
}

GLFWwindow* voxel_engine::window::get_window() const
{
    return _window;
}
