#ifndef MESH_HPP
#define MESH_HPP

#include "../vulkan/Buffer.hpp"
#include <filesystem>
#include <memory>
#include <vector>

#include "Vertex.hpp"
#include "Drawable.hpp"

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

private:
    uint32_t m_numVertices;
    std::unique_ptr<vulkan::Buffer> m_vertexBuffer;
    uint32_t m_numIndices;
    std::unique_ptr<vulkan::Buffer> m_indexBuffer;
};
} // namespace pvk::geometry

#endif // MESH_HPP
