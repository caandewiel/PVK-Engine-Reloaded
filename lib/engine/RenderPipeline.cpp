#include "RenderPipeline.hpp"

#include <array>
#include <memory>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan.hpp>

#include "../vulkan/Helper.hpp"

#include "Graphics.hpp"

namespace pvk::engine
{
RenderPipeline::RenderPipeline()
{
    m_renderPass = std::make_unique<pvk::vulkan::RenderPass>();
    m_frameBuffer = std::make_unique<pvk::vulkan::FrameBuffer>(*m_renderPass);
    m_commandPool = std::make_unique<pvk::vulkan::CommandPool>();
    m_commandBuffer = std::make_unique<pvk::command_buffer::CommandBuffer>(*m_commandPool);
    m_renderFence = std::make_unique<pvk::vulkan::Fence>();
    m_presentSemaphore = std::make_unique<pvk::vulkan::Semaphore>();
    m_renderSemaphore = std::make_unique<pvk::vulkan::Semaphore>();
    m_renderStageUI = std::make_unique<pvk::ui::RenderStageUI>(*m_commandPool, *m_renderPass);
}

RenderPipeline::~RenderPipeline()
{
    m_renderStageUI.reset();
    m_renderFence.reset();
    m_renderSemaphore.reset();
    m_presentSemaphore.reset();
    m_frameBuffer.reset();
    m_commandBuffer.reset();
    m_commandPool.reset();
    m_renderPass.reset();
}

void RenderPipeline::render()
{
    const auto imageIndex = this->getNextImageFromSwapChain();

    const auto &commandBuffer = m_commandBuffer->getCommandBuffer(0);
    commandBuffer.reset();
    commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    std::array<vk::ClearValue, 2> clearValues = {};
    clearValues[0].setColor(vk::ClearColorValue(std::array<float, 4>{1.0F, 1.0F, 1.0F, 1.0F}));
    clearValues[1].depthStencil.setDepth(1.0F);

    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.setRenderPass(m_renderPass->getRenderPass());
    renderPassBeginInfo.setFramebuffer(m_frameBuffer->getFrameBuffer(imageIndex));
    renderPassBeginInfo.renderArea.offset = vk::Offset2D(0, 0);
    renderPassBeginInfo.renderArea.extent = graphics::get()->getSwapChain().getSwapChainExtent();
    renderPassBeginInfo.setClearValues(clearValues);

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    render(getCommandBuffer());

    m_renderStageUI->render(getCommandBuffer());

    commandBuffer.endRenderPass();
    commandBuffer.end();

    this->submitCommandBufferToGraphicsQueue();
    this->presentGraphicsQueue(imageIndex);
}

void RenderPipeline::registerRenderStage(const std::string &identifier, std::unique_ptr<Shader> &&renderStage)
{
    m_renderStages.insert(std::make_pair(identifier, std::move(renderStage)));
}

const vulkan::RenderPass &RenderPipeline::getRenderPass() const
{
    return *m_renderPass;
}

const vulkan::CommandPool &RenderPipeline::getCommandPool() const
{
    return *m_commandPool;
}

const command_buffer::CommandBuffer &RenderPipeline::getCommandBuffer() const
{
    return *m_commandBuffer;
}

const vulkan::FrameBuffer &RenderPipeline::getFrameBuffer() const
{
    return *m_frameBuffer;
}

const vulkan::Pipeline &RenderPipeline::getPipeline() const
{
    return *m_pipeline;
}

const ui::RenderStageUI &RenderPipeline::getRenderStageUI() const
{
    return *m_renderStageUI;
}

void RenderPipeline::log(const std::string &content) const
{
    m_renderStageUI->getLogger().addLog(content.c_str());
}

void RenderPipeline::registerWidget(const std::string &identifier, std::unique_ptr<ui::Widget> &&widget)
{
    m_renderStageUI->registerWidget(identifier, std::move(widget));
}

uint32_t RenderPipeline::getNextImageFromSwapChain()
{
    const auto &logicalDevice = graphics::get()->getDevice().getLogicalDevice();
    const auto waitForFences =
        logicalDevice.waitForFences(1, &m_renderFence->getFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
    pvk::vulkan::assertVulkan(waitForFences, "Could not wait for rendering fence.");

    logicalDevice.resetFences(m_renderFence->getFence());

    try
    {
        const auto acquireNextImage = logicalDevice.acquireNextImageKHR(graphics::get()->getSwapChain().getSwapChain(),
                                                                        std::numeric_limits<uint64_t>::max(),
                                                                        m_presentSemaphore->getSemaphore(),
                                                                        nullptr);

        return pvk::vulkan::assertVulkan(acquireNextImage, "Could not acquire next message.");
    }
    catch (const vk::OutOfDateKHRError &exception)
    {
        // @TODO: Implement this.
        return 0;
    }
    catch (const vk::SystemError &exception)
    {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }
}

void RenderPipeline::submitCommandBufferToGraphicsQueue()
{
    vk::SubmitInfo submitInfo;
    std::array<vk::PipelineStageFlags, 1> waitStages{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setCommandBuffers(m_commandBuffer->getCommandBuffer(0));
    submitInfo.setWaitDstStageMask(waitStages);
    submitInfo.setWaitSemaphores(m_presentSemaphore->getSemaphore());
    submitInfo.setSignalSemaphores(m_renderSemaphore->getSemaphore());

    graphics::get()->getDevice().getGraphicsQueue().submit(submitInfo, m_renderFence->getFence());
}

void RenderPipeline::presentGraphicsQueue(uint32_t imageIndex)
{
    vk::PresentInfoKHR presentInfo;
    presentInfo.setSwapchains(graphics::get()->getSwapChain().getSwapChain());
    presentInfo.setWaitSemaphores(m_renderSemaphore->getSemaphore());
    presentInfo.setImageIndices(imageIndex);

    pvk::vulkan::assertVulkan(graphics::get()->getDevice().getGraphicsQueue().presentKHR(presentInfo),
                              "Could not present image to SwapChain.");
}

void RenderPipeline::setScene(std::weak_ptr<geometry::Scene> scene)
{
    m_scene = std::move(scene);
}

const geometry::Scene &RenderPipeline::getScene() const
{
    return *m_scene.lock();
}

void RenderPipeline::setCamera(std::weak_ptr<engine::Camera> camera)
{
    m_camera = std::move(camera);
}

const engine::Camera &RenderPipeline::getCamera() const
{
    return *m_camera.lock();
}

engine::Camera &RenderPipeline::getCamera()
{
    return *m_camera.lock();
}
} // namespace pvk::engine