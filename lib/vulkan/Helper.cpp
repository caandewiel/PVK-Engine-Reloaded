#include "Helper.hpp"
#include "../engine/Graphics.hpp"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Fence.hpp"
#include <limits>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::helper
{
void assertVulkan(const vk::Result &result, std::string &&message)
{
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error(message);
    }
}

void executeOneTimeCommandBuffer(
    const pvk::vulkan::CommandPool &commandPool,
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function)
{
    auto commandBuffer = std::make_unique<command_buffer::CommandBuffer>(commandPool);
    commandBuffer->getCommandBuffer(0).begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    function(*commandBuffer);

    commandBuffer->getCommandBuffer(0).end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer->getCommandBuffer(0));

    graphics::get()->getDevice().getGraphicsQueue().submit(submitInfo, nullptr);
    graphics::get()->getDevice().getLogicalDevice().waitIdle();
}

void executeOneTimeCommandBuffer(
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function)
{
    auto commandBuffer = std::make_unique<command_buffer::CommandBuffer>(
        graphics::get()->getRenderPipeline<pvk::engine::RenderPipeline>().getCommandPool());
    commandBuffer->getCommandBuffer(0).begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    function(*commandBuffer);

    commandBuffer->getCommandBuffer(0).end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer->getCommandBuffer(0));

    graphics::get()->getDevice().getGraphicsQueue().submit(submitInfo, nullptr);
    graphics::get()->getDevice().getLogicalDevice().waitIdle();
}
} // namespace pvk::helper