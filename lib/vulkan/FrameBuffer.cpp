#include "FrameBuffer.hpp"
#include "../engine/Graphics.hpp"
#include <memory>
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
FrameBuffer::FrameBuffer(const vulkan::RenderPass &renderPass)
{
    const auto &swapChain = graphics::get()->getSwapChain();
    const auto &device = graphics::get()->getDevice();

    for (size_t i = 0; i < swapChain.getSwapChainImages().size(); i++)
    {
        try
        {
            vk::FramebufferCreateInfo frameBufferCreateInfo;
            frameBufferCreateInfo.setFlags(vk::FramebufferCreateFlags());
            frameBufferCreateInfo.setRenderPass(renderPass.getRenderPass());
            frameBufferCreateInfo.setAttachments(swapChain.getSwapChainImageView(i));
            frameBufferCreateInfo.setWidth(swapChain.getSwapChainExtent().width);
            frameBufferCreateInfo.setHeight(swapChain.getSwapChainExtent().height);
            frameBufferCreateInfo.setLayers(1);

            this->m_frameBuffers.emplace_back(device.getLogicalDevice().createFramebuffer(frameBufferCreateInfo));
        }
        catch (const vk::SystemError &exception)
        {
            throw std::runtime_error("Failed to create framebuffer.");
        }
    }
}

FrameBuffer::~FrameBuffer()
{
    for (const auto &frameBuffer : m_frameBuffers)
    {
        graphics::get()->getDevice().getLogicalDevice().destroyFramebuffer(frameBuffer);
    }
}

const vk::Framebuffer &FrameBuffer::getFrameBuffer(uint32_t index) const
{
    return m_frameBuffers.at(index);
}
} // namespace pvk::vulkan