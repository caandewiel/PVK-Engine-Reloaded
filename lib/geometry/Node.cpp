#include "Node.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "../engine/exceptions/PvkExceptionAsset.hpp"
#include "glm/fwd.hpp"

namespace pvk::geometry
{
Node::Node(std::string &&name, const glm::mat4 &matrix, std::vector<std::weak_ptr<Mesh>> &&meshes)
    : m_name(std::move(name)), m_matrix(matrix), m_meshes(std::move(meshes))
{
}

bool Node::hasParent() const
{
    return m_parent.lock() != nullptr;
}

bool Node::hasMesh() const
{
    return !m_meshes.empty();
}

const std::string &Node::getName() const
{
    return m_name;
}

const std::vector<std::weak_ptr<Node>> &Node::getChildren() const
{
    return m_children;
}

const Node &Node::getParent() const
{
    if (!hasParent())
    {
        throw PvkExceptionAsset("Trying to get parent from node which has no parent node.");
    }

    return *m_parent.lock();
}

Node &Node::getParent()
{
    return *m_parent.lock();
}

const std::vector<std::weak_ptr<Mesh>> &Node::getMeshes() const
{
    if (!hasMesh())
    {
        throw PvkExceptionAsset("Trying to get mesh indices from node which has no meshes.");
    }

    return m_meshes;
}

const glm::mat4 &Node::getMatrix() const
{
    return m_matrix;
}

void Node::setParent(std::weak_ptr<Node> parent)
{
    m_parent = std::move(parent);
}

void Node::addChild(std::weak_ptr<Node> child)
{
    m_children.push_back(std::move(child));
}

void Node::draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const
{
    for (const auto &mesh : m_meshes)
    {
        mesh.lock()->draw(commandBuffer);
    }

    for (const auto &child : m_children)
    {
        child.lock()->draw(commandBuffer);
    }
}

} // namespace pvk::geometry
