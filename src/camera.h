#pragma once
#include <imgui.h>
#include <glm/glm.hpp>
#include "window.h"
#include "common/types.h"

namespace voxel_engine
{
    class camera
    {
    private:
        static camera* _current_camera;
        window& _window;
        float64_t _last_x = 0.0f;
        float64_t _last_y = 0.0f;
        bool _is_captured = false;

    public:
        float32_t fov = 45.0f;
        float32_t yaw = -90.0f;
        float32_t pitch = 0.0f;
        float32_t near = 0.01f;
        float32_t far = 500000.0f;

        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 position;

        explicit camera(window& _window);

        void contrain_angles();
        void update_angles();
        void update_vectors();

        void look_at(const glm::vec3& _position);
        void update();
        void capture();
        void release();
        void update_position();
        void update_mouse_position();

        [[nodiscard]] glm::mat4 get_inverse_view_matrix() const;
        [[nodiscard]] glm::mat4 get_view_matrix() const;
        [[nodiscard]] glm::mat4 get_projection_matrix() const;
        [[nodiscard]] static camera* get_current_camera();

        static void update_camera(const window& _window, camera& _camera, const ImGuiIO* _io)
        {
            if (!_io->WantCaptureMouse && (glfwGetMouseButton(_window.get_window(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ||
            glfwGetMouseButton(_window.get_window(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS))
            {
                _camera.capture();
            }
            else
            {
                _camera.release();
            }
            if (glfwGetKey(_window.get_window(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(_window.get_window(), true);
            }
            _camera.update_position();
        }
    };
}
