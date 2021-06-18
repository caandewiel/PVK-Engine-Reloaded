#ifndef MESH_HPP
#define MESH_HPP

#include <filesystem>
#include <memory>
#include <vector>

#include "Vertex.hpp"
#include "Drawable.hpp"

#include "../vulkan/Buffer.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"

namespace pvk::geometry
{
class Mesh : public Drawable
{
public:
    explicit Mesh(const std::filesystem::path &path);

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

private:
    uint32_t m_numVertices;
    std::unique_ptr<vulkan::Buffer> m_vertexBuffer;
    uint32_t m_numIndices;
    std::unique_ptr<vulkan::Buffer> m_indexBuffer;

    glm::vec3 m_minBounds;
    glm::vec3 m_maxBounds;
};
} // namespace pvk::geometry

#endif // MESH_HPP
