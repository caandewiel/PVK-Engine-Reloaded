#include "MemoryAllocator.hpp"
#include "../engine/Graphics.hpp"
#include <memory>

#define VMA_IMPLEMENTATION
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "../../external/proxy/vk_mem_alloc.h"

namespace pvk::vulkan
{
MemoryAllocator::MemoryAllocator()
{
    this->m_allocator = std::make_unique<VmaAllocator>();

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.instance = static_cast<VkInstance>(graphics::get()->getInstance().getInstance());
    allocatorCreateInfo.physicalDevice =
        static_cast<VkPhysicalDevice>(graphics::get()->getDevice().getPhysicalDevice());
    allocatorCreateInfo.device = static_cast<VkDevice>(graphics::get()->getDevice().getLogicalDevice());
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;

    vmaCreateAllocator(&allocatorCreateInfo, m_allocator.get());
}

MemoryAllocator::~MemoryAllocator()
{
    vmaDestroyAllocator(*m_allocator);
}

const VmaAllocator& MemoryAllocator::getAllocator() const
{
    return *m_allocator;
}

std::unique_ptr<Buffer> MemoryAllocator::createBuffer(const pvk::geometry::Mesh &mesh) const
{
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
    bufferCreateInfo.setSize(mesh.getSize());

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    return createBuffer(bufferCreateInfo, allocationCreateInfo);
}

std::unique_ptr<Buffer> MemoryAllocator::createBuffer(const vk::BufferCreateInfo &bufferCreateInfo,
                                                      const VmaAllocationCreateInfo &allocationCreateInfo) const
{
    VkBuffer buffer = {};
    VmaAllocation allocation = {};
    const auto bufferCreateInfoBackwardsCompatible = static_cast<const VkBufferCreateInfo>(bufferCreateInfo);
    vmaCreateBuffer(
        *m_allocator, &bufferCreateInfoBackwardsCompatible, &allocationCreateInfo, &buffer, &allocation, nullptr);

    return std::make_unique<pvk::vulkan::Buffer>(std::move(buffer), std::move(allocation));
}
} // namespace pvk::vulkan