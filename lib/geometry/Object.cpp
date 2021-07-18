#include "Object.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <utility>

#include <fmt/format.h>

#include "../asset/Blueprint.hpp"

#include "../engine/exceptions/PvkExceptionAsset.hpp"
#include "glm/fwd.hpp"

namespace
{
std::pair<glm::vec3, glm::vec3> calculateBounds(
    const absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> &meshLookup)
{
    glm::vec3 minBounds = {
        std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    glm::vec3 maxBounds = {
        std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};

    for (const auto &[_, mesh] : meshLookup)
    {
        auto currentBounds = mesh->getBounds();

        if (minBounds.x > currentBounds.first.x)
        {
            minBounds.x = currentBounds.first.x;
        }

        if (minBounds.y > currentBounds.first.y)
        {
            minBounds.y = currentBounds.first.y;
        }

        if (minBounds.z > currentBounds.first.z)
        {
            minBounds.z = currentBounds.first.z;
        }

        if (maxBounds.x < currentBounds.second.x)
        {
            maxBounds.x = currentBounds.second.x;
        }

        if (maxBounds.y < currentBounds.second.y)
        {
            maxBounds.y = currentBounds.second.y;
        }

        if (maxBounds.z < currentBounds.second.z)
        {
            maxBounds.z = currentBounds.second.z;
        }
    }

    return std::make_pair(minBounds, maxBounds);
}
} // namespace

namespace pvk::geometry
{
Object::Object(absl::flat_hash_map<uint32_t, std::shared_ptr<Mesh>> &&meshLookup,
               absl::flat_hash_map<uint32_t, std::shared_ptr<Node>> &&nodeLookup,
               absl::flat_hash_map<uint32_t, std::shared_ptr<Material>> &&materialLookup)
    : m_meshLookup(std::move(meshLookup)), m_nodeLookup(std::move(nodeLookup)),
      m_materialLookup(std::move(materialLookup))
{
    auto bounds = calculateBounds(m_meshLookup);
    m_minBounds = bounds.first;
    m_maxBounds = bounds.second;
}

const Mesh &Object::getMesh(uint32_t meshIndex) const
{
    try
    {
        return *m_meshLookup.at(meshIndex);
    }
    catch (const std::out_of_range &exception)
    {
        throw pvk::PvkExceptionAsset(fmt::format("Invalid mesh index '{}'", meshIndex));
    }
}

size_t Object::getNumberOfMeshes() const
{
    return m_meshLookup.size();
}

const Node &Object::getNode(uint32_t nodeIndex) const
{
    try
    {
        return *m_nodeLookup.at(nodeIndex);
    }
    catch (const std::out_of_range &exception)
    {
        throw pvk::PvkExceptionAsset(fmt::format("Invalid node index '{}'", nodeIndex));
    }
}

const Node &Object::getRootNode() const
{
    return *m_nodeLookup.at(0);
}

void Object::draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const
{
    getRootNode().draw(commandBuffer);
}

void Object::pushConstant(const std::string &identifier, void *data)
{
    m_pushConstants.insert(std::make_pair(identifier, data));
}

const void *Object::getPushConstantData(const std::string &identifier) const
{
    return m_pushConstants.at(identifier);
}

std::pair<glm::vec3, glm::vec3> Object::getBounds() const
{
    return std::make_pair(m_minBounds, m_maxBounds);
}
} // namespace pvk::geometry