#include "Pipeline.hpp"

#include "../engine/Graphics.hpp"
#include "../io/io.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"
#include <array>
#include <memory>
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
namespace
{
vk::ShaderModule createShaderModule(const vulkan::Device &device, const std::string &shader)
{
    const auto shaderContent = io::readFile(shader);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.setCode(shaderContent);

    return device.getLogicalDevice().createShaderModule(shaderModuleCreateInfo);
}

vk::PipelineLayout createPipelineLayout(const vulkan::Device &device)
{
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};

    return device.getLogicalDevice().createPipelineLayout(pipelineLayoutCreateInfo);
}

vk::PipelineViewportStateCreateInfo initializeViewportStateCreateInfo(const vulkan::SwapChain &swapChain,
                                                                      const std::array<vk::Viewport, 1> &viewports,
                                                                      const std::array<vk::Rect2D, 1> &scissors)
{

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.setViewports(viewports);
    viewportStateCreateInfo.setScissors(scissors);

    return viewportStateCreateInfo;
}

vk::PipelineColorBlendAttachmentState initializeColorBlendAttachmentState()
{
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendAttachmentState.setBlendEnable(VK_FALSE);

    return colorBlendAttachmentState;
}

vk::PipelineColorBlendStateCreateInfo initializeColorBlendStateCreateInfo(
    const vk::PipelineColorBlendAttachmentState &colorBlendAttachmentState)
{
    vk::PipelineColorBlendStateCreateInfo colorBlendingStateCreateInfo;
    colorBlendingStateCreateInfo.setLogicOpEnable(VK_FALSE);
    colorBlendingStateCreateInfo.setLogicOp(vk::LogicOp::eCopy);
    colorBlendingStateCreateInfo.setAttachments(colorBlendAttachmentState);

    return colorBlendingStateCreateInfo;
}

vk::PipelineRasterizationStateCreateInfo initializeRasterizationStateCreateInfo()
{
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.setPolygonMode(vk::PolygonMode::eFill);
    rasterizationStateCreateInfo.setLineWidth(1.0F);
    rasterizationStateCreateInfo.setDepthBiasEnable(VK_FALSE);
    rasterizationStateCreateInfo.setDepthBiasClamp(0.0F);
    rasterizationStateCreateInfo.setDepthBiasConstantFactor(0.0F);
    rasterizationStateCreateInfo.setDepthBiasSlopeFactor(0.0F);
    rasterizationStateCreateInfo.setRasterizerDiscardEnable(VK_FALSE);
    rasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eNone);
    rasterizationStateCreateInfo.setFrontFace(vk::FrontFace::eClockwise);

    return rasterizationStateCreateInfo;
}

vk::PipelineMultisampleStateCreateInfo initializeMultisampleStateCreateInfo()
{
    return {};
}

vk::PipelineInputAssemblyStateCreateInfo initializeInputAssemblyStateCreateInfo()
{
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssemblyStateCreateInfo.setPrimitiveRestartEnable(VK_FALSE);

    return inputAssemblyStateCreateInfo;
}

vk::PipelineVertexInputStateCreateInfo initializeVertexInputStateCreateInfo()
{
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.setVertexBindingDescriptions(pvk::geometry::vertex::bindingDescriptions);
    vertexInputStateCreateInfo.setVertexAttributeDescriptions(pvk::geometry::vertex::attributeDescriptions);

    return vertexInputStateCreateInfo;
}
} // namespace

Pipeline::Pipeline(const vulkan::RenderPass &renderPass,
                   const std::string &vertexShader,
                   const std::string &fragmentShader)
{
    const auto &device = graphics::get()->getDevice();
    const auto &swapChain = graphics::get()->getSwapChain();

    m_pipelineLayout = createPipelineLayout(device);
    auto vertexShaderModule = createShaderModule(device, vertexShader);
    auto fragmentShaderModule = createShaderModule(device, fragmentShader);

    std::array<vk::Viewport, 1> viewports = {vk::Viewport(0.0F,
                                                          0.0F,
                                                          static_cast<float>(swapChain.getSwapChainExtent().width),
                                                          static_cast<float>(swapChain.getSwapChainExtent().height),
                                                          0.0F,
                                                          1.0F)};
    std::array<vk::Rect2D, 1> scissors = {vk::Rect2D(vk::Offset2D(0, 0), swapChain.getSwapChainExtent())};

    const auto colorBlendAttachmentState = initializeColorBlendAttachmentState();
    const auto colorBlendingStateCreateInfo = initializeColorBlendStateCreateInfo(colorBlendAttachmentState);
    const auto viewportStateCreateInfo = initializeViewportStateCreateInfo(swapChain, viewports, scissors);
    const auto rasterizationStateCreateInfo = initializeRasterizationStateCreateInfo();
    const auto multisampleStateCreateInfo = initializeMultisampleStateCreateInfo();
    const auto inputAssemblyStateCreateInfo = initializeInputAssemblyStateCreateInfo();
    const auto vertexInputStateCreateInfo = initializeVertexInputStateCreateInfo();

    vk::PipelineShaderStageCreateInfo vertexShaderStage;
    vertexShaderStage.setFlags(vk::PipelineShaderStageCreateFlags());
    vertexShaderStage.setStage(vk::ShaderStageFlagBits::eVertex);
    vertexShaderStage.setModule(vertexShaderModule);
    vertexShaderStage.setPName("main");

    vk::PipelineShaderStageCreateInfo fragmentShaderStage;
    fragmentShaderStage.setFlags(vk::PipelineShaderStageCreateFlags());
    fragmentShaderStage.setStage(vk::ShaderStageFlagBits::eFragment);
    fragmentShaderStage.setModule(fragmentShaderModule);
    fragmentShaderStage.setPName("main");

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    shaderStages.emplace_back(vertexShaderStage);
    shaderStages.emplace_back(fragmentShaderStage);

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.setStages(shaderStages);
    pipelineCreateInfo.setLayout(m_pipelineLayout);
    pipelineCreateInfo.setRenderPass(renderPass.getRenderPass());
    pipelineCreateInfo.setPRasterizationState(&rasterizationStateCreateInfo);
    pipelineCreateInfo.setPViewportState(&viewportStateCreateInfo);
    pipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);
    pipelineCreateInfo.setPColorBlendState(&colorBlendingStateCreateInfo);
    pipelineCreateInfo.setPVertexInputState(&vertexInputStateCreateInfo);
    pipelineCreateInfo.setPInputAssemblyState(&inputAssemblyStateCreateInfo);
    pipelineCreateInfo.setSubpass(0);

    auto pipeline = device.getLogicalDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo);

    if (pipeline.result == vk::Result::eSuccess)
    {
        this->m_pipeline = pipeline.value;

        std::vector<vk::ShaderModule> shaderModules;
        this->m_shaderModules.emplace_back(vertexShaderModule);
        this->m_shaderModules.emplace_back(fragmentShaderModule);
    }
    else
    {
    }
}

Pipeline::~Pipeline()
{
    auto logicalDevice = graphics::get()->getDevice().getLogicalDevice();

    for (const auto &shaderModule : this->m_shaderModules)
    {
        logicalDevice.destroyShaderModule(shaderModule);
    }

    logicalDevice.destroyPipeline(this->m_pipeline);
    logicalDevice.destroyPipelineLayout(this->m_pipelineLayout);
}

const vk::Pipeline &Pipeline::getPipeline() const
{
    return m_pipeline;
}
} // namespace pvk::vulkan