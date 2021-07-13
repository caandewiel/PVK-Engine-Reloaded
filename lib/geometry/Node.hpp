#ifndef NODE_HPP
#define NODE_HPP

#include <memory>

#include <glm/glm.hpp>

#include "Drawable.hpp"
#include "Mesh.hpp"

namespace pvk::geometry
{
class Node : Drawable
{
public:
    Node(std::string &&name, const glm::mat4 &matrix, std::vector<std::weak_ptr<Mesh>> &&meshes);

    [[nodiscard]] bool hasParent() const;
    [[nodiscard]] bool hasMesh() const;
    [[nodiscard]] bool hasChildren() const;

    [[nodiscard]] const std::string &getName() const;
    [[nodiscard]] const std::vector<std::weak_ptr<Node>> &getChildren() const;
    [[nodiscard]] const Node &getParent() const;
    [[nodiscard]] Node &getParent();
    [[nodiscard]] const std::vector<std::weak_ptr<Mesh>> &getMeshes() const;
    [[nodiscard]] const glm::mat4 &getMatrix() const;

    void setParent(std::weak_ptr<Node> parent);
    void addChild(std::weak_ptr<Node> child);

    void draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const override;

private:
    std::string m_name;
    glm::mat4 m_matrix;
    std::vector<std::weak_ptr<Mesh>> m_meshes;
    std::weak_ptr<Node> m_parent;
    std::vector<std::weak_ptr<Node>> m_children{};
};
} // namespace pvk::geometry

#endif // NODE_HPP
