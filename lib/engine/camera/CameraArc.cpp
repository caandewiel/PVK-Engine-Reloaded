#include "CameraArc.hpp"
#include <cmath>
#include <vulkan/vulkan.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

namespace pvk::engine
{

CameraArc::CameraArc(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up)
    : m_position(position), m_lookAt(lookAt), m_up(up)
{
    updateViewMatrix();
}

glm::mat4 CameraArc::getViewMatrix() const
{
    return m_viewMatrix;
}

glm::vec3 CameraArc::getPosition() const
{
    return m_position;
}

void CameraArc::update(const vk::Extent2D &viewport, float deltaX, float deltaY)
{
    const auto angleX = static_cast<float>(2 * M_PI / viewport.width) * deltaX;
    const auto angleY = static_cast<float>(M_PI / viewport.height) * deltaY;

    glm::vec3 forward = glm::normalize(m_position - m_lookAt);
    glm::vec3 left = glm::normalize(glm::cross(m_up, forward));
    m_up = glm::cross(forward, left);
    const glm::vec4 pivot = {m_lookAt, 1.0F};
    glm::vec4 position = {m_position, 1.0F};

    const auto rotationMatrixX = glm::rotate(glm::mat4(1.0F), -angleX, m_up);
    position = (rotationMatrixX * (position - pivot)) + pivot;

    const auto rotationMatrixY = glm::rotate(glm::mat4(1.0F), angleY, left);
    position = (rotationMatrixY * (position - pivot)) + pivot;

    m_position = position;

    updateViewMatrix();
}

void CameraArc::updateViewMatrix()
{
    m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
    // m_right = glm::transpose(m_viewMatrix)[0];
    // m_up = glm::cross(m_lookAt, m_right);
}

} // namespace pvk::engine
