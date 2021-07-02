#include "Entity.hpp"

#include <memory>
#include <utility>

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>

namespace pvk::engine
{
Transform::Transform()
{
    m_transform.m_position = glm::vec3(0.0F);
    m_transform.m_scale = glm::vec3(1.0F);
    m_transform.m_rotation = glm::vec3(0.0F);

    initializeUniformBuffer();
}

Transform::Transform(glm::vec3 position)
{
    m_transform.m_position = position;
    m_transform.m_scale = glm::vec3(1.0F);
    m_transform.m_rotation = glm::vec3(0.0F);

    initializeUniformBuffer();
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

void Transform::initializeUniformBuffer()
{
    m_uniformBuffer = std::make_unique<UniformBuffer>(sizeof(m_transform));
    updateUniformBuffer();
}

void Transform::updateUniformBuffer()
{
    m_uniformBuffer->update(&m_transform);
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
} // namespace pvk::engine