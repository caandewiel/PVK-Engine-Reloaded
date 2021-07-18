#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <memory>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include "../shader/UniformBuffer.hpp"

namespace pvk::geometry
{
class Object;
}

namespace pvk::engine
{
class Transform
{
public:
    Transform();
    Transform(glm::vec3 position);

    [[nodiscard]] const glm::vec3 &getPosition() const;
    [[nodiscard]] const glm::vec3 &getScale() const;
    [[nodiscard]] const glm::vec3 &getRotation() const;
    [[nodiscard]] const glm::mat4 &getTransformMatrix() const;

private:
    void updateTransformMatrix();

    struct
    {
        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::vec3 m_rotation;
        glm::mat4 m_transformation;
    } m_transform;
};

class Entity
{
public:
    Entity(std::shared_ptr<geometry::Object> object);
    Entity(std::shared_ptr<geometry::Object> object, Transform transform);

    [[nodiscard]] const geometry::Object &getObject() const;
    [[nodiscard]] const glm::mat4 &getTransformMatrix() const;

private:
    std::shared_ptr<geometry::Object> m_object;
    Transform m_transform;
};
} // namespace pvk::engine

#endif // __ENTITY_H__