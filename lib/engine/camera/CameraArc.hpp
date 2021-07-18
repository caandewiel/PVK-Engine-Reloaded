#ifndef __CAMERAARC_H__
#define __CAMERAARC_H__

#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>

#include "Camera.hpp"

namespace pvk::engine
{
class CameraArc : public Camera
{
public:
    CameraArc(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::vec3 getPosition() const;
    void update(const vk::Extent2D &viewport, float deltaX, float deltaY);

private:
    void updateViewMatrix();

    glm::vec3 m_position;
    glm::vec3 m_lookAt;
    glm::vec3 m_up;
    // glm::vec3 m_right;
    glm::mat4 m_viewMatrix;
};
} // namespace pvk::engine

#endif // __CAMERAARC_H__