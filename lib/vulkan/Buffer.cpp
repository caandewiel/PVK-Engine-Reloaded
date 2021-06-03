#include "Buffer.hpp"
#include "../engine/Graphics.hpp"
#include "../../external/proxy/vk_mem_alloc.h"
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
Buffer::Buffer(vk::Buffer &&buffer, VmaAllocation &&allocation)
    : m_buffer(buffer), m_allocation(allocation)
{
}

Buffer::~Buffer() 
{
    vmaDestroyBuffer(graphics::get()->getMemoryAllocator().getAllocator(), m_buffer, m_allocation);
}

const vk::Buffer& Buffer::getBuffer() const
{
    return m_buffer;
}

const VmaAllocation& Buffer::getAllocation() const
{
    return m_allocation;
}
} // namespace pvk::vulkan