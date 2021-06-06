#include "Mesh.hpp"

#include <fstream>
#include <iostream>
#include <span>
#include <type_traits>

#include <fmt/core.h>

#include <vulkan/vulkan.hpp>

#include "../../external/proxy/vk_mem_alloc.h"

#include "../engine/Graphics.hpp"
#include "../io/AssetLoader.hpp"
#include "../vulkan/Helper.hpp"

namespace pvk::geometry
{
Mesh::Mesh(const std::filesystem::path &path)
{
    pvk::log(fmt::format("Loading mesh {}", path.c_str()));
    auto [vertices, indices] = pvk::io::loadMeshBuffers(path);
    m_numVertices = vertices.size();
    m_numIndices = indices.size();

    pvk::log(fmt::format("Mesh has {} vertices and {} indices\n", getNumberOfVertices(), getNumberOfIndices()));

    m_vertexBuffer = pvk::vulkan::createBuffer(
        std::move(vertices), vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
    m_indexBuffer = pvk::vulkan::createBuffer(
        std::move(indices), vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
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