#include "ShaderBuilder.hpp"

#include <memory>
#include <vulkan/vulkan.hpp>

#include "ShaderReflection.hpp"

#include "../Graphics.hpp"

#include "../../io/io.hpp"

#include "../../vulkan/DescriptorPool.hpp"
#include "../../vulkan/Device.hpp"
#include "../../vulkan/SwapChain.hpp"

namespace
{
vk::ShaderModule createShaderModule(const pvk::vulkan::Device &device, const std::string &shader)
{
    const auto shaderContent = pvk::io::readFile(shader);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.setCode(shaderContent);

    return device.getLogicalDevice().createShaderModule(shaderModuleCreateInfo);
}

vk::PipelineViewportStateCreateInfo initializeViewportStateCreateInfo(const pvk::vulkan::SwapChain &swapChain,
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

vk::PipelineDepthStencilStateCreateInfo initializeDepthStencilStateCreateInfo()
{
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    depthStencilStateCreateInfo.setDepthTestEnable(VK_TRUE);
    depthStencilStateCreateInfo.setDepthWriteEnable(VK_TRUE);
    depthStencilStateCreateInfo.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
    depthStencilStateCreateInfo.setDepthBoundsTestEnable(VK_FALSE);
    depthStencilStateCreateInfo.setMinDepthBounds(0.0F);
    depthStencilStateCreateInfo.setMaxDepthBounds(1.0F);
    depthStencilStateCreateInfo.setStencilTestEnable(VK_FALSE);

    return depthStencilStateCreateInfo;
}
} // namespace

namespace
{
std::vector<vk::DescriptorSetLayout> initializeDescriptorSetLayouts(
    const pvk::engine::pipeline::PipelineDefinition &pipelineDefinition)
{
    const auto &device = pvk::graphics::get()->getDevice().getLogicalDevice();

    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{};

    for (const auto &descriptorSet : pipelineDefinition.descriptorSets)
    {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
        descriptorSetLayoutCreateInfo.setBindings(descriptorSet.second);

        descriptorSetLayouts.emplace_back(device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
    }

    return descriptorSetLayouts;
}

std::vector<vk::PushConstantRange> initializePushConstantRanges(
    const pvk::engine::pipeline::PipelineDefinition &pipelineDefinition)
{
    std::vector<vk::PushConstantRange> pushConstantRanges;

    for (const auto &[identifier, descriptor] : pipelineDefinition.pushConstantDefinitions)
    {
        vk::PushConstantRange pushConstantRange;
        pushConstantRange.setStageFlags(descriptor.shaderStage);
        pushConstantRange.setSize(descriptor.bufferSize);
        pushConstantRange.setOffset(0);

        pushConstantRanges.emplace_back(pushConstantRange);
    }

    return pushConstantRanges;
}

vk::PipelineLayout initializePipelineLayout(const pvk::engine::pipeline::PipelineDefinition &pipelineDefinition,
                                            const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts,
                                            const std::vector<vk::PushConstantRange> &pushConstantRanges)
{
    const auto &device = pvk::graphics::get()->getDevice().getLogicalDevice();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.setSetLayouts(descriptorSetLayouts);
    pipelineLayoutCreateInfo.setPushConstantRanges(pushConstantRanges);

    return device.createPipelineLayout(pipelineLayoutCreateInfo);
}
} // namespace

namespace pvk::engine::pipeline
{
ShaderBuilder &ShaderBuilder::setVertexShader(const std::filesystem::path &vertexShader)
{
    this->m_pathToVertexShader = vertexShader;

    return *this;
}

ShaderBuilder &ShaderBuilder::setFragmentShader(const std::filesystem::path &fragmentShader)
{
    this->m_pathToFragmentShader = fragmentShader;

    return *this;
}

ShaderBuilder &ShaderBuilder::setRenderPass(const vulkan::RenderPass &renderPass)
{
    this->m_renderPass = &renderPass;

    return *this;
}

ShaderBuilder &ShaderBuilder::setViewport(const vk::Extent2D &viewport)
{
    this->m_viewport = viewport;

    return *this;
}

std::unique_ptr<pvk::engine::Shader> ShaderBuilder::create()
{
    const auto &device = graphics::get()->getDevice();
    const auto &swapChain = graphics::get()->getSwapChain();

    auto pipelineDefinition = createPipelineDefinitionFromSPIRVShader(m_pathToVertexShader, m_pathToFragmentShader);
    auto descriptorSetLayouts = initializeDescriptorSetLayouts(pipelineDefinition);
    auto pushConstantRanges = initializePushConstantRanges(pipelineDefinition);
    auto pipelineLayout = initializePipelineLayout(pipelineDefinition, descriptorSetLayouts, pushConstantRanges);

    auto vertexShaderModule = createShaderModule(device, m_pathToVertexShader);
    auto fragmentShaderModule = createShaderModule(device, m_pathToFragmentShader);

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
    const auto depthStencilStateCreateInfo = initializeDepthStencilStateCreateInfo();

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
    pipelineCreateInfo.setLayout(pipelineLayout);
    pipelineCreateInfo.setRenderPass(m_renderPass->getRenderPass());
    pipelineCreateInfo.setPRasterizationState(&rasterizationStateCreateInfo);
    pipelineCreateInfo.setPViewportState(&viewportStateCreateInfo);
    pipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);
    pipelineCreateInfo.setPColorBlendState(&colorBlendingStateCreateInfo);
    pipelineCreateInfo.setPVertexInputState(&vertexInputStateCreateInfo);
    pipelineCreateInfo.setPInputAssemblyState(&inputAssemblyStateCreateInfo);
    pipelineCreateInfo.setPDepthStencilState(&depthStencilStateCreateInfo);
    pipelineCreateInfo.setSubpass(0);

    auto vulkanPipeline = device.getLogicalDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo);

    if (vulkanPipeline.result == vk::Result::eSuccess)
    {
        std::vector<vk::ShaderModule> shaderModules{};
        shaderModules.emplace_back(vertexShaderModule);
        shaderModules.emplace_back(fragmentShaderModule);

        return std::make_unique<pvk::engine::Shader>(
            std::make_unique<pvk::vulkan::Pipeline>(
                vulkanPipeline.value, pipelineLayout, std::move(shaderModules), std::move(descriptorSetLayouts)),
            std::move(pipelineDefinition.descriptorDefinitions));
    }

    throw std::runtime_error("BOEM");
}
} // namespace pvk::engine::pipeline