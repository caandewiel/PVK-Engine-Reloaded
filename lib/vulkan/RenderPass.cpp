#include "RenderPass.hpp"
#include <memory>
#include <type_traits>
#include <vulkan/vulkan.hpp>
#include "../engine/Graphics.hpp"

namespace pvk::vulkan
{
RenderPass::RenderPass()
{
    vk::AttachmentDescription colorAttachment;
    colorAttachment.setFormat(graphics::get()->getSwapChain().getSurfaceFormat().format);
    colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
    colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentDescription depthAttachment = {};
    depthAttachment.setFormat(graphics::get()->getDevice().findSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment));
    depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
    depthAttachment.setLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference colorAttachmentRef = {0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::AttachmentReference depthAttachmentRef = {1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachments(colorAttachmentRef);
    subpass.setPDepthStencilAttachment(&depthAttachmentRef);

    vk::SubpassDependency dependency;
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
    dependency.setDstSubpass(0);
    dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

    std::vector<vk::AttachmentDescription> attachments = {colorAttachment, depthAttachment};

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachments(attachments);
    renderPassInfo.setSubpasses(subpass);
    renderPassInfo.setDependencies(dependency);

    try
    {
        m_renderPass = graphics::get()->getDevice().getLogicalDevice().createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError &error)
    {
        throw std::runtime_error("Failed to create render pass");
    }
}

RenderPass::~RenderPass()
{
    graphics::get()->getDevice().getLogicalDevice().destroyRenderPass(m_renderPass);
}

const vk::RenderPass &RenderPass::getRenderPass() const
{
    return m_renderPass;
}
} // namespace pvk::vulkan