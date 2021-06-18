#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pvk::engine
{
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position), m_front(glm::vec3(0.0F, 0.0F, -1.0F)), m_worldUp(up), m_yaw(yaw), m_pitch(pitch), m_movementSpeed(defaultSpeed),
      m_sensitivity(defaultSensitivity)
{
    calculateCameraVectors();
}

void Camera::update(CameraMovement direction, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;

    if (direction == FORWARD)
    {
        m_position += m_front * velocity;
    }
    if (direction == BACKWARD)
    {
        m_position -= m_front * velocity;
    }
    if (direction == LEFT)
    {
        m_position -= m_right * velocity;
    }
    if (direction == RIGHT)
    {
        m_position += m_right * velocity;
    }
}

void Camera::update(float xOffset, float yOffset)
{
    xOffset *= m_sensitivity;
    yOffset *= m_sensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    m_pitch = std::fmaxf(-cameraPitchLimit, std::fminf(m_pitch, cameraPitchLimit));

    calculateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::vec3 Camera::getPosition() const
{
    return m_position;
}

void Camera::calculateCameraVectors()
{
    glm::vec3 front{
        cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
        sin(glm::radians(m_pitch)),
        sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
    };

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
} // namespace pvk::engine