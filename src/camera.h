#ifndef VOXELENGINE_CAMERA_H
#define VOXELENGINE_CAMERA_H

#include "window.h"
#include "glm/glm.hpp"

namespace VoxelEngine
{
    class Camera
    {
    private:
        Window* m_Window = nullptr;
        f64 m_LastX = 0.0f;
        f64 m_LastY = 0.0f;

        f32 m_Fov = 45.0f;
        f64 m_Yaw = -90.0f;
        f64 m_Pitch = 0.0f;
        bool m_IsCaptured = false;

        glm::vec3 m_Front{0.0f, 0.0f, -1.0f};
        glm::vec3 m_Right{1.0f, 0.0f, 0.0f};
        glm::vec3 m_Up{0.0f, 1.0f, 0.0f};
        glm::vec3 m_Position{0.0f, 0.0f, 0.0f};

        void contrainAngles();
        void updateAngles();
        void updateVectors();
        void updateMousePosition();
    public:
        explicit Camera(Window* p_Window);

        void lookAt(const glm::vec3& p_Position);
        void update();
        void capture();
        void release();
        void updatePosition();
        void resetMousePosition();

        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] glm::mat4 getProjectionMatrix() const;
        [[nodiscard]] glm::vec3 getPosition() const;
    };
}

#endif
