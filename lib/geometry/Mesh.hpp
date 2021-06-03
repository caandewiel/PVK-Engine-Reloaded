#ifndef MESH_HPP
#define MESH_HPP

#include "../vulkan/Buffer.hpp"
#include <memory>
#include <vector>

#include "Vertex.hpp"

namespace pvk::geometry
{
class Mesh
{
public:
    explicit Mesh(std::vector<Vertex> &&vertices);
    explicit Mesh(const std::string &path);

    [[nodiscard]] const vulkan::Buffer &getVertexBuffer() const;
    [[nodiscard]] const vulkan::Buffer &getIndexBuffer() const;

    [[nodiscard]] size_t getSize() const
    {
        return m_vertices.size() * sizeof(Vertex);
    }

    [[nodiscard]] size_t getNumberOfVertices() const
    {
        return m_vertices.size();
    }

    [[nodiscard]] size_t getNumberOfIndices() const
    {
        return m_indices.size();
    }

private:
    std::vector<Vertex> m_vertices;
    std::unique_ptr<vulkan::Buffer> m_vertexBuffer;
    std::vector<uint32_t> m_indices;
    std::unique_ptr<vulkan::Buffer> m_indexBuffer;
};
} // namespace pvk::geometry

#endif // MESH_HPP
