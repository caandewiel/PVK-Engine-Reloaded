#include "Scene.hpp"
#include <memory>

namespace pvk::geometry
{
void Scene::addObject(std::shared_ptr<Object> object)
{
    m_objects.emplace_back(std::move(object));
}

void Scene::addEntity(std::unique_ptr<engine::Entity> &&entity)
{
    m_entities.emplace_back(std::move(entity));
}

const std::vector<std::shared_ptr<Object>> &Scene::getObjects() const
{
    return m_objects;
}

const std::vector<std::unique_ptr<engine::Entity>> &Scene::getEntities() const
{
    return m_entities;
}

} // namespace pvk::geometry
