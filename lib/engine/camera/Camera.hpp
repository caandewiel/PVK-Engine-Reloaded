#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include "glm/fwd.hpp"

namespace pvk::engine
{
enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

constexpr float defaultYaw = -90.0F;
constexpr float defaultPitch = 0.0F;
constexpr float defaultSpeed = 5.0F;
constexpr float defaultSensitivity = 0.05F;
constexpr float defaultZoom = 45.0F;

constexpr float cameraPitchLimit = 89.0F;

class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw = defaultYaw, float pitch = defaultPitch);
    void update(CameraMovement direction, float deltaTime);
    void update(float xOffset, float yOffset);
    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::vec3 getPosition() const;

private:
    glm::vec3 m_position, m_front, m_up {}, m_right {}, m_worldUp;
    float m_yaw, m_pitch;
    float m_movementSpeed, m_sensitivity;

    void calculateCameraVectors();
};

} // namespace pvk::engine

#endif // CAMERA_HPP
