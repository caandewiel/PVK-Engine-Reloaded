#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "Object.hpp"

#include "../engine/entity/Entity.hpp"

namespace pvk::geometry
{
class Scene
{
public:
    void addObject(std::shared_ptr<Object> object);
    void addEntity(std::unique_ptr<engine::Entity> &&entity);
    [[nodiscard]] const std::vector<std::shared_ptr<Object>> &getObjects() const;
    [[nodiscard]] const std::vector<std::unique_ptr<engine::Entity>> &getEntities() const;

private:
    std::vector<std::shared_ptr<Object>> m_objects;
    std::vector<std::unique_ptr<engine::Entity>> m_entities;
};
} // namespace pvk::geometry

#endif // __SCENE_H__