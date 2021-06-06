#ifndef NODE_HPP
#define NODE_HPP

#include <glm/glm.hpp>
#include <memory>

#include "Mesh.hpp"

namespace pvk::geometry
{
class Node
{
public:
    Node(const glm::mat4 &matrix, std::vector<uint32_t> &&meshIndices);

    [[nodiscard]] bool hasParent() const;
    [[nodiscard]] bool hasMesh() const;

    [[nodiscard]] const Node& getParent() const;
    [[nodiscard]] const std::vector<uint32_t>& getMeshIndices() const;
    [[nodiscard]] const glm::mat4& getMatrix() const;
    void setParent(std::weak_ptr<Node> parent);
private:
    glm::mat4 m_matrix;
    std::vector<uint32_t> m_meshIndices;
    std::weak_ptr<Node> m_parent;
};
} // namespace pvk::geometry

#endif // NODE_HPP
