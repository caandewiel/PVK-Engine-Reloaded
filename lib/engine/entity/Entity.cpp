#include "Entity.hpp"

#include <cmath>
#include <memory>
#include <utility>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/trigonometric.hpp>

namespace pvk::engine
{
Transform::Transform()
{
    m_transform.m_position = glm::vec3(0.0F);
    m_transform.m_scale = glm::vec3(1.0F);
    m_transform.m_rotation = glm::vec3(0.0F);
    updateTransformMatrix();
}

Transform::Transform(glm::vec3 position)
{
    m_transform.m_position = position;
    m_transform.m_scale = glm::vec3(1.0F);
    m_transform.m_rotation = glm::vec3(0.0F);
    updateTransformMatrix();
}

const glm::vec3 &Transform::getPosition() const
{
    return m_transform.m_position;
}

const glm::vec3 &Transform::getScale() const
{
    return m_transform.m_scale;
}

const glm::vec3 &Transform::getRotation() const
{
    return m_transform.m_rotation;
}

const glm::mat4 &Transform::getTransformMatrix() const
{
    return m_transform.m_transformation;
}

void Transform::updateTransformMatrix()
{
    m_transform.m_transformation = glm::mat4(1.0F);
    m_transform.m_transformation = glm::translate(m_transform.m_transformation, m_transform.m_position);
    m_transform.m_transformation = glm::scale(m_transform.m_transformation, m_transform.m_scale);
    m_transform.m_transformation *= glm::yawPitchRoll(m_transform.m_rotation.x, m_transform.m_rotation.y, m_transform.m_rotation.z);
}

Entity::Entity(std::shared_ptr<geometry::Object> object) : m_object(std::move(object))
{
    m_transform = {};
}

Entity::Entity(std::shared_ptr<geometry::Object> object, Transform transform)
    : m_object(std::move(object)), m_transform(std::move(transform))
{
}

const geometry::Object &Entity::getObject() const
{
    return *m_object;
}

const glm::mat4 &Entity::getTransformMatrix() const
{
    return m_transform.getTransformMatrix();
}
} // namespace pvk::engine