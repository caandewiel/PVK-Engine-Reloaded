#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include "glm/fwd.hpp"

#include <vulkan/vulkan.hpp>

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
    Camera() = default;
    virtual void update(const vk::Extent2D &viewport, float deltaX, float deltaY) = 0;
    [[nodiscard]] virtual glm::mat4 getViewMatrix() const = 0;
    [[nodiscard]] virtual glm::vec3 getPosition() const = 0;

private:
};

} // namespace pvk::engine

#endif // CAMERA_HPP
