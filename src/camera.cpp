#include "camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

void scroll_callback(GLFWwindow* p_Window, float64_t p_X, float64_t p_Y)
{
    (void) p_Window;
    (void) p_X;

    voxel_engine::camera* camera = voxel_engine::camera::get_current_camera();
    camera->position += camera->front * static_cast<float32_t>(p_Y * 10.0f);
}

namespace voxel_engine
{
    camera* camera::_current_camera = nullptr;

    camera::camera(window& _window)
        : _window(_window)
        , front(0.0f, 0.0f, -1.0f)
        , right(1.0f, 0.0f, 0.0f)
        , up(0.0f, 1.0f, 0.0f)
    {
        _current_camera = this;
        update_mouse_position();
        glfwSetScrollCallback(_window.get_window(), &scroll_callback);
    }

    void camera::contrain_angles()
    {
        if (pitch > 89.0f)
        {
            pitch = 89.0f;
        }
        else if (pitch < -89.0f)
        {
            pitch = -89.0f;
        }
    }

    void camera::update_angles()
    {
        const float64_t curr_x = _last_x;
        const float64_t curr_y = _last_y;

        update_mouse_position();

        yaw += (_last_x - curr_x) * 0.1f;
        pitch -= (_last_y - curr_y) * 0.1f;
        contrain_angles();
    }

    void camera::update_vectors()
    {
        const float64_t rad_yaw = glm::radians(yaw);
        const float64_t rad_pitch = glm::radians(pitch);

        glm::vec3 direction;
        direction.x = glm::cos(rad_yaw) * glm::cos(rad_pitch);
        direction.y = glm::sin(rad_pitch);
        direction.z = glm::sin(rad_yaw) * glm::cos(rad_pitch);
        front = glm::normalize(direction);
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }

    void camera::update_position()
    {
        float32_t speed = _window.get_delta_time() * 10.0f;

        if (glfwGetKey(_window.get_window(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            speed *= 0.1;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_W) == GLFW_PRESS)
        {
            position += front * speed;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_S) == GLFW_PRESS)
        {
            position -= front * speed;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_A) == GLFW_PRESS)
        {
            position -= right * speed;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_D) == GLFW_PRESS)
        {
            position += right * speed;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_E) == GLFW_PRESS)
        {
            position += up * speed;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_Q) == GLFW_PRESS)
        {
            position -= up * speed;
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            yaw -= speed;
            contrain_angles();
            update_vectors();
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            yaw += speed;
            contrain_angles();
            update_vectors();
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_UP) == GLFW_PRESS)
        {
            pitch += speed;
            contrain_angles();
            update_vectors();
        }
        if (glfwGetKey(_window.get_window(), GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            pitch -= speed;
            contrain_angles();
            update_vectors();
        }
    }

    void camera::update_mouse_position()
    {
        glfwGetCursorPos(_window.get_window(), &_last_x, &_last_y);
    }

    void camera::update()
    {
        update_angles();
        update_vectors();
    }

    void camera::look_at(const glm::vec3& _position)
    {
        if (_position == position)
        {
            return;
        }

        update_mouse_position();

        const glm::vec3 direction = glm::normalize(_position - position);
        yaw = glm::degrees(glm::atan(direction.z, direction.x));
        pitch = glm::degrees(glm::asin(direction.y));
        update();
    }

    void camera::capture()
    {
        if (!_is_captured)
        {
            glfwSetInputMode(_window.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            update_mouse_position();
            _is_captured = true;
        }
        update();
    }

    void camera::release()
    {
        if (_is_captured)
        {
            glfwSetInputMode(_window.get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            _is_captured = false;
        }
    }

    glm::mat4 camera::get_view_matrix() const
    {
        return glm::lookAt(position, position + front, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 camera::get_inverse_view_matrix() const
    {
        return glm::inverse(get_view_matrix());
    }

    glm::mat4 camera::get_projection_matrix() const
    {
        return glm::perspective(
            glm::radians(fov),
            static_cast<float32_t>(_window.width) / static_cast<float32_t>(_window.height),
            near,
            far
        );
    }

    camera* camera::get_current_camera()
    {
        return _current_camera;
    }
}

