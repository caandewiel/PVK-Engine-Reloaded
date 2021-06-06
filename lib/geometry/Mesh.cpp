#include "Mesh.hpp"

#include "../../external/proxy/vk_mem_alloc.h"
#include "../engine/Graphics.hpp"

#include "../io/AssetLoader.hpp"
#include "fmt/core.h"

#include <fstream>
#include <iostream>
#include <span>
#include <type_traits>

#include <vulkan/vulkan.hpp>

namespace pvk::geometry
{
Mesh::Mesh(std::vector<Vertex> &&vertices) : m_vertices(std::move(vertices))
{
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
    bufferCreateInfo.setSize(m_vertices.size() * sizeof(Vertex));

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    m_vertexBuffer = graphics::get()->getMemoryAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo);

    void *data;
    vmaMapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_vertexBuffer->getAllocation(), &data);

    memcpy(data, m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    vmaUnmapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_vertexBuffer->getAllocation());
}

Mesh::Mesh(const std::string &path)
{
    pvk::log(fmt::format("Loading mesh {}", path));
    auto [vertices, indices] = pvk::io::loadMeshBuffers(path);
    m_vertices = std::move(vertices);
    m_indices = std::move(indices);

    pvk::log(fmt::format("Mesh has {} vertices and {} indices\n", getNumberOfVertices(), getNumberOfIndices()));

    {
        vk::BufferCreateInfo vertexBufferCreateInfo{};
        vertexBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        vertexBufferCreateInfo.setSize(m_vertices.size() * sizeof(Vertex));

        VmaAllocationCreateInfo vertexAllocationCreateInfo = {};
        vertexAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        m_vertexBuffer =
            graphics::get()->getMemoryAllocator().createBuffer(vertexBufferCreateInfo, vertexAllocationCreateInfo);

        void *data;
        vmaMapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_vertexBuffer->getAllocation(), &data);

        memcpy(data, m_vertices.data(), m_vertices.size() * sizeof(Vertex));

        vmaUnmapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_vertexBuffer->getAllocation());
    }

    {
        vk::BufferCreateInfo indexBufferCreateInfo{};
        indexBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
        indexBufferCreateInfo.setSize(m_indices.size() * sizeof(uint32_t));

        VmaAllocationCreateInfo indexAllocationCreateInfo = {};
        indexAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        m_indexBuffer =
            graphics::get()->getMemoryAllocator().createBuffer(indexBufferCreateInfo, indexAllocationCreateInfo);

        void *data;
        vmaMapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_indexBuffer->getAllocation(), &data);

        memcpy(data, m_indices.data(), m_indices.size() * sizeof(uint32_t));

        vmaUnmapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_indexBuffer->getAllocation());
    }
}

const vulkan::Buffer &Mesh::getVertexBuffer() const
{
    return *m_vertexBuffer;
}

const vulkan::Buffer &Mesh::getIndexBuffer() const
{
    return *m_indexBuffer;
}
} // namespace pvk::geometry