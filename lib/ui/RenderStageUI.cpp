#include "RenderStageUI.hpp"
#include "../../external/imgui/backends/imgui_impl_glfw.h"
#include "../../external/imgui/backends/imgui_impl_vulkan.h"
#include "../../external/imgui/imgui.h"
#include "../engine/Graphics.hpp"
#include "../vulkan/Helper.hpp"
#include "LogWidget.hpp"
#include "OverlayWidget.hpp"

#include <array>
#include <memory>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace pvk::ui
{
RenderStageUI::RenderStageUI(const vulkan::CommandPool &commandPool, const vulkan::RenderPass &renderPass)
{
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        {vk::DescriptorType::eSampler, defaultDescriptorCount},
        {vk::DescriptorType::eCombinedImageSampler, defaultDescriptorCount},
        {vk::DescriptorType::eSampledImage, defaultDescriptorCount},
        {vk::DescriptorType::eStorageImage, defaultDescriptorCount},
        {vk::DescriptorType::eUniformTexelBuffer, defaultDescriptorCount},
        {vk::DescriptorType::eStorageTexelBuffer, defaultDescriptorCount},
        {vk::DescriptorType::eUniformBuffer, defaultDescriptorCount},
        {vk::DescriptorType::eStorageBuffer, defaultDescriptorCount},
        {vk::DescriptorType::eUniformBufferDynamic, defaultDescriptorCount},
        {vk::DescriptorType::eStorageBufferDynamic, defaultDescriptorCount},
        {vk::DescriptorType::eInputAttachment, defaultDescriptorCount},
    };

    m_descriptorPool = std::make_unique<vulkan::DescriptorPool>(poolSizes);

    m_context = ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(graphics::get()->getWindow().getWindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = graphics::get()->getInstance().getInstance();
    initInfo.PhysicalDevice = graphics::get()->getDevice().getPhysicalDevice();
    initInfo.Device = graphics::get()->getDevice().getLogicalDevice();
    initInfo.Queue = graphics::get()->getDevice().getGraphicsQueue();
    initInfo.DescriptorPool = m_descriptorPool->getDescriptorPool();
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = 3;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&initInfo, renderPass.getRenderPass());

    vulkan::executeOneTimeCommandBuffer(commandPool, [](const command_buffer::CommandBuffer &commandBuffer) {
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer.getCommandBuffer(0));
    });

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    m_logWidget = std::make_unique<LogWidget>();
    m_overlayWidget = std::make_unique<OverlayWidget>("General information");
}

RenderStageUI::~RenderStageUI() 
{
    ImGui_ImplVulkan_Shutdown();
    m_descriptorPool.reset();
    m_pipeline.reset();
}

void RenderStageUI::render(const command_buffer::CommandBuffer &commandBuffer) const
{
    ImGui::SetCurrentContext(m_context);
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    m_logWidget->draw();
    m_overlayWidget->draw();

    for (const auto &[_, widget] : m_widgets) {
        widget->draw();
    }

    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.getCommandBuffer(0));
}

void RenderStageUI::log(const std::string &text)
{
    m_logWidget->addLog(text.c_str());
}

void RenderStageUI::registerWidget(const std::string &identifier, std::unique_ptr<Widget> &&widget) 
{
    m_widgets.insert(std::make_pair(identifier, std::move(widget)));
}

LogWidget &RenderStageUI::getLogger() const
{
    return *m_logWidget;
}

OverlayWidget& RenderStageUI::getOverlayWidget() const
{
    return *m_overlayWidget;
}
} // namespace pvk::ui