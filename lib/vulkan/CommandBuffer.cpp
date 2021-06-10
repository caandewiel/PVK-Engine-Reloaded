#include "CommandBuffer.hpp"

#include <memory>
#include <type_traits>

#include <vulkan/vulkan.hpp>

#include "../engine/Graphics.hpp"

namespace pvk::command_buffer
{
CommandBuffer::CommandBuffer(const vulkan::CommandPool &commandPool)
{
    vk::CommandBufferAllocateInfo allocateInfo(commandPool.getCommandPool(), vk::CommandBufferLevel::ePrimary, 1);

    this->m_commandBuffers = graphics::get()->getDevice().getLogicalDevice().allocateCommandBuffers(allocateInfo);
}

const vk::CommandBuffer &CommandBuffer::getCommandBuffer(uint32_t index) const
{
    return m_commandBuffers.at(index);
}
} // namespace pvk::command_buffer