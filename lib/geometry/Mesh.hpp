#ifndef MESH_HPP
#define MESH_HPP

#include <filesystem>
#include <memory>
#include <utility>
#include <vector>

#include "Drawable.hpp"
#include "Material.hpp"
#include "Vertex.hpp"

#include "../engine/render_stage/Shader.hpp"
#include "../engine/shader/Descriptor.hpp"
#include "../vulkan/Buffer.hpp"

namespace pvk::geometry
{
class Object;

class Mesh : public Drawable
{
public:
    explicit Mesh(const std::filesystem::path &path);
    Mesh(std::unique_ptr<vulkan::Buffer> &&vertexBuffer,
         std::unique_ptr<vulkan::Buffer> &&indexBuffer,
         size_t numberOfVertices,
         size_t numberOfIndices,
         std::pair<glm::vec3, glm::vec3> bounds);

    [[nodiscard]] const vulkan::Buffer &getVertexBuffer() const;
    [[nodiscard]] const vulkan::Buffer &getIndexBuffer() const;

    [[nodiscard]] size_t getSize() const
    {
        return m_numVertices * sizeof(Vertex);
    }

    [[nodiscard]] size_t getNumberOfVertices() const
    {
        return m_numVertices;
    }

    [[nodiscard]] size_t getNumberOfIndices() const
    {
        return m_numIndices;
    }

    void draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const override;

    [[nodiscard]] std::pair<glm::vec3, glm::vec3> getBounds() const;

    [[nodiscard]] const geometry::Material &getMaterial() const;
    void setMaterial(std::weak_ptr<geometry::Material> material);

    friend void engine::Shader::bindObjectDescriptor(
        const geometry::Object &object,
        const std::string &identifier,
        const std::function<const engine::Descriptor &(const geometry::Object &object, const geometry::Mesh &mesh)>
            &callback);

private:
    std::unique_ptr<vulkan::Buffer> m_vertexBuffer;
    std::unique_ptr<vulkan::Buffer> m_indexBuffer;

    std::weak_ptr<geometry::Material> m_material;

    uint32_t m_numVertices;
    uint32_t m_numIndices;

    glm::vec3 m_minBounds;
    glm::vec3 m_maxBounds;
};
} // namespace pvk::geometry

#endif // MESH_HPP
