#include "Pipeline.hpp"

#include <array>
#include <memory>
#include <type_traits>

#include <vulkan/vulkan.hpp>

#include "RenderPass.hpp"
#include "SwapChain.hpp"

#include "../engine/Graphics.hpp"
#include "../io/io.hpp"

namespace pvk::vulkan
{
Pipeline::Pipeline(const vk::Pipeline &pipeline,
                   const vk::PipelineLayout &pipelineLayout,
                   std::vector<vk::ShaderModule> &&shaderModules,
                   std::vector<vk::DescriptorSetLayout> &&descriptorSetLayouts)
    : m_pipeline(pipeline), m_pipelineLayout(pipelineLayout), m_shaderModules(std::move(shaderModules)),
      m_descriptorSetLayouts(std::move(descriptorSetLayouts))
{
}

Pipeline::~Pipeline()
{
    for (auto &descriptorSetLayout : m_descriptorSetLayouts) {
        graphics::get()->getDevice().getLogicalDevice().destroyDescriptorSetLayout(descriptorSetLayout);
    }

    for (auto &shaderModule : m_shaderModules)
    {
        graphics::get()->getDevice().getLogicalDevice().destroyShaderModule(shaderModule);
    }

    graphics::get()->getDevice().getLogicalDevice().destroyPipelineLayout(m_pipelineLayout);
    graphics::get()->getDevice().getLogicalDevice().destroyPipeline(m_pipeline);
}

const vk::DescriptorSetLayout& Pipeline::getDescriptorSetLayout(uint32_t index) const
{
    return m_descriptorSetLayouts.at(index);
}

const vk::Pipeline &Pipeline::getPipeline() const
{
    return m_pipeline;
}

const vk::PipelineLayout& Pipeline::getPipelineLayout() const
{
    return m_pipelineLayout;
}
} // namespace pvk::vulkan