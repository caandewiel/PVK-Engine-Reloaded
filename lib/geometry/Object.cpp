#include "Object.hpp"

#include <fstream>
#include <stdexcept>

#include <fmt/format.h>

#include "../asset/Blueprint.hpp"

#include "../engine/exceptions/PvkExceptionAsset.hpp"

namespace pvk::geometry
{
Object::Object(absl::flat_hash_map<uint32_t, std::unique_ptr<Mesh>> &&meshLookup,
               absl::flat_hash_map<uint32_t, std::shared_ptr<Node>> &&nodeLookup)
    : m_meshLookup(std::move(meshLookup)), m_nodeLookup(std::move(nodeLookup))
{
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
} // namespace pvk::geometry
