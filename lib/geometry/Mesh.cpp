#include "Mesh.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <utility>
#include <span>

#include <fmt/core.h>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "../engine/Graphics.hpp"
#include "../io/AssetLoader.hpp"
#include "../vulkan/Helper.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"

namespace
{
std::pair<glm::vec3, glm::vec3> calculateBounds(const std::vector<pvk::geometry::Vertex> &vertices)
{
    const auto x = std::minmax_element(
        vertices.begin(), vertices.end(), [](const pvk::geometry::Vertex &a, const pvk::geometry::Vertex &b) {
            return a.position.x < b.position.x;
        });

    const auto y = std::minmax_element(
        vertices.begin(), vertices.end(), [](const pvk::geometry::Vertex &a, const pvk::geometry::Vertex &b) {
            return a.position.y < b.position.y;
        });

    const auto z = std::minmax_element(
        vertices.begin(), vertices.end(), [](const pvk::geometry::Vertex &a, const pvk::geometry::Vertex &b) {
            return a.position.z < b.position.z;
        });

    return std::make_pair(glm::vec3(x.first->position.x, y.first->position.y, z.first->position.z),
                          glm::vec3(x.second->position.x, y.second->position.y, z.second->position.z));
}
} // namespace

namespace pvk::geometry
{
Mesh::Mesh(const std::filesystem::path &path)
{
    pvk::log(fmt::format("Loading mesh {}", path.c_str()));
    auto [vertices, indices] = pvk::io::loadMeshBuffers(path);
    m_numVertices = vertices.size();
    m_numIndices = indices.size();

    pvk::log(fmt::format("Mesh has {} vertices and {} indices\n", getNumberOfVertices(), getNumberOfIndices()));

    auto bounds = calculateBounds(vertices);
    m_minBounds = bounds.first;
    m_maxBounds = bounds.second;

    m_vertexBuffer = pvk::vulkan::createBuffer(
        std::move(vertices), vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
    m_indexBuffer = pvk::vulkan::createBuffer(
        std::move(indices), vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
}

Mesh::Mesh(std::unique_ptr<vulkan::Buffer> &&vertexBuffer,
           std::unique_ptr<vulkan::Buffer> &&indexBuffer,
           size_t numberOfVertices,
           size_t numberOfIndices,
           std::pair<glm::vec3, glm::vec3> bounds)
    : m_vertexBuffer(std::move(vertexBuffer)), m_indexBuffer(std::move(indexBuffer)), m_numVertices(numberOfVertices),
      m_numIndices(numberOfIndices)
{
    m_minBounds = bounds.first;
    m_maxBounds = bounds.second;
}

const vulkan::Buffer &Mesh::getVertexBuffer() const
{
    return *m_vertexBuffer;
}

const vulkan::Buffer &Mesh::getIndexBuffer() const
{
    return *m_indexBuffer;
}

void Mesh::draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const
{
    const auto offset = 0;

    const auto &cb = commandBuffer.getCommandBuffer(0);
    cb.bindVertexBuffers(0, m_vertexBuffer->getBuffer(), offset);
    cb.bindIndexBuffer(m_indexBuffer->getBuffer(), offset, vk::IndexType::eUint32);
    cb.drawIndexed(getNumberOfIndices(), 1, 0, 0, 0);
}

std::pair<glm::vec3, glm::vec3> Mesh::getBounds() const
{
    return std::make_pair(m_minBounds, m_maxBounds);
}

void Mesh::setMaterial(std::weak_ptr<geometry::Material> material)
{
    m_material = std::move(material);
}

const geometry::Material &Mesh::getMaterial() const
{
    return *m_material.lock();
}
} // namespace pvk::geometry