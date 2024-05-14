#include "camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

namespace VoxelEngine
{
    Camera::Camera(Window* p_Window)
    {
        m_Window = p_Window;
        resetMousePosition();
        update();
    }

    void Camera::resetMousePosition()
    {
        m_LastX = 0.0f;
        m_LastY = 0.0f;
        glfwSetCursorPos(m_Window->getWindow(), m_LastX, m_LastY);
    }

    void Camera::contrainAngles()
    {
        if (m_Pitch > 89.99f)
        {
            m_Pitch = 89.99f;
        }
        else if (m_Pitch < -89.99f)
        {
            m_Pitch = -89.99f;
        }
    }

    void Camera::updateAngles()
    {
        f64 curr_x = m_LastX;
        f64 curr_y = m_LastY;

        updateMousePosition();

        m_Yaw += (m_LastX - curr_x) * 0.1f;
        m_Pitch -= (m_LastY - curr_y) * 0.1f;
        contrainAngles();
    }

    void Camera::updateVectors()
    {
        f64 rad_yaw = glm::radians(m_Yaw);
        f64 rad_pitch = glm::radians(m_Pitch);

        glm::vec3 direction;
        direction.x = glm::cos(rad_yaw) * glm::cos(rad_pitch);
        direction.y = glm::sin(rad_pitch);
        direction.z = glm::sin(rad_yaw) * glm::cos(rad_pitch);
        m_Front = glm::normalize(direction);
        m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera::updatePosition()
    {
        f32 speed = m_Window->getDeltaTime() * 20.0f;

        if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        {
            m_Position += m_Front * speed;
        }
        if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        {
            m_Position -= m_Front * speed;
        }
        if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            m_Position -= m_Right * speed;
        }
        if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            m_Position += m_Right * speed;
        }
        if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_E) == GLFW_PRESS)
        {
            m_Position += m_Up * speed;
        }
        if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
        {
            m_Position -= m_Up * speed;
        }
    }

    void Camera::updateMousePosition()
    {
        glfwGetCursorPos(m_Window->getWindow(), &m_LastX, &m_LastY);
    }

    void Camera::update()
    {
        updateAngles();
        updateVectors();
    }

    void Camera::lookAt(const glm::vec3& p_Position)
    {
        resetMousePosition();

        glm::vec3 direction = glm::normalize(p_Position - m_Position);
        m_Yaw = glm::degrees(glm::atan(direction.z, direction.x));
        m_Pitch = glm::degrees(glm::asin(direction.y));
        update();
    }

    void Camera::capture()
    {
        if (!m_IsCaptured)
        {
            glfwSetInputMode(m_Window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            resetMousePosition();
            m_IsCaptured = true;
        }
        update();
    }

    void Camera::release()
    {
        if (m_IsCaptured)
        {
            glfwSetInputMode(m_Window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_IsCaptured = false;
        }
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(m_Position, m_Position + m_Front, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 Camera::getProjectionMatrix() const
    {
        int width, height;
        glfwGetWindowSize(m_Window->getWindow(), &width, &height);
        // fmt::println("{}, {}", width, height);
        return glm::perspective(glm::radians(m_Fov), float(width) / float(height), 0.1f, 5000.0f);
    }

    glm::vec3 Camera::getPosition() const
    {
        return m_Position;
    }
}
