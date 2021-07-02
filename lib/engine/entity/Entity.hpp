#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <memory>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include "../shader/UniformBuffer.hpp"

#include "../../geometry/Object.hpp"

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

private:
    struct
    {
        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::vec3 m_rotation;
    } m_transform;

    std::unique_ptr<UniformBuffer> m_uniformBuffer;

    void initializeUniformBuffer();
    void updateUniformBuffer();
};

class Entity
{
public:
    Entity(std::shared_ptr<geometry::Object> object);
    Entity(std::shared_ptr<geometry::Object> object, Transform transform);

    [[nodiscard]] const geometry::Object &getObject() const;

private:
    std::shared_ptr<geometry::Object> m_object;
    Transform m_transform;
};
} // namespace pvk::engine

#endif // __ENTITY_H__