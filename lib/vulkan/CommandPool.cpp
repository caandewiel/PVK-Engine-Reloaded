#include "CommandPool.hpp"
#include <memory>

#include <vulkan/vulkan.hpp>

#include "../engine/Graphics.hpp"

namespace pvk::vulkan
{
CommandPool::CommandPool()
{
    const auto &device = graphics::get()->getDevice();

    vk::CommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    commandPoolCreateInfo.setQueueFamilyIndex(device.getQueueFamilyIndices().graphicsFamily.value());

    this->m_commandPool = device.getLogicalDevice().createCommandPool(commandPoolCreateInfo);
}

CommandPool::~CommandPool()
{
    graphics::get()->getDevice().getLogicalDevice().destroyCommandPool(m_commandPool);
}

const vk::CommandPool &CommandPool::getCommandPool() const
{
    return m_commandPool;
}
} // namespace pvk::vulkan