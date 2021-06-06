#include "Node.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "../engine/exceptions/PvkExceptionAsset.hpp"

namespace pvk::geometry
{
Node::Node(const glm::mat4 &matrix, std::vector<uint32_t> &&meshIndices)
    : m_matrix(matrix), m_meshIndices(std::move(meshIndices))
{
}

bool Node::hasParent() const
{
    return m_parent.lock() != nullptr;
}

bool Node::hasMesh() const
{
    return !m_meshIndices.empty();
}

const Node &Node::getParent() const
{
    if (!hasParent())
    {
        throw PvkExceptionAsset("Trying to get parent from node which has no parent node.");
    }

    return *m_parent.lock();
}

const std::vector<uint32_t> &Node::getMeshIndices() const
{
    if (!hasMesh())
    {
        throw PvkExceptionAsset("Trying to get mesh indices from node which has no meshes.");
    }

    return m_meshIndices;
}

const glm::mat4 &Node::getMatrix() const
{
    return m_matrix;
}

void Node::setParent(std::weak_ptr<Node> parent)
{
    m_parent = std::move(parent);
}

} // namespace pvk::geometry
