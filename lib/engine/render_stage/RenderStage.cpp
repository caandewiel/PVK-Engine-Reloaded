#include "RenderStage.hpp"

#include <memory>
#include <utility>

#include <vulkan/vulkan.hpp>

#include "../Graphics.hpp"
#include "glm/fwd.hpp"

namespace pvk::engine
{
RenderStage::RenderStage(std::unique_ptr<pvk::vulkan::Pipeline> &&pipeline,
                         std::unique_ptr<pvk::vulkan::DescriptorPool> &&descriptorPool,
                         absl::flat_hash_map<std::string, pipeline::DescriptorDefinition> &&descriptorDefinitions)
    : m_pipeline(std::move(pipeline)), m_descriptorPool(std::move(descriptorPool)),
      m_descriptorDefinitions(std::move(descriptorDefinitions))
{
}

RenderStage::~RenderStage()
{
    m_descriptorPool.reset();
    m_pipeline.reset();
}

void RenderStage::renderObject(const command_buffer::CommandBuffer &commandBuffer, const geometry::Object &object) const
{
    const auto &cb = commandBuffer.getCommandBuffer(0);
    cb.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline->getPipeline());
    cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline->getPipelineLayout(), 0, 1, &m_descriptorSets.at(0), 0, nullptr);
    cb.pushConstants(m_pipeline->getPipelineLayout(), vk::ShaderStageFlagBits::eAllGraphics, 0, sizeof(glm::mat4), &object.getRootNode().getMatrix());
    object.draw(commandBuffer);
}

void RenderStage::bindDescriptor(const std::string &identifier, const Descriptor &descriptor)
{
    try
    {
        const auto &device = graphics::get()->getDevice().getLogicalDevice();

        auto descriptorDefinition = m_descriptorDefinitions.at(identifier);
        auto descriptorSetLayout = m_pipeline->getDescriptorSetLayout(descriptorDefinition.descriptorSetIndex);
        std::vector<vk::DescriptorSetLayout> layouts(Context::getNumberOfSwapChainImages(), descriptorSetLayout);

        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
        descriptorSetAllocateInfo.setDescriptorPool(m_descriptorPool->getDescriptorPool());
        descriptorSetAllocateInfo.setSetLayouts(descriptorSetLayout);
        descriptorSetAllocateInfo.setDescriptorSetCount(1);

        auto descriptorSets = device.allocateDescriptorSets(descriptorSetAllocateInfo);
        m_descriptorSets.insert(std::make_pair(descriptorDefinition.descriptorSetIndex, *descriptorSets.begin()));

        auto writeDescriptorSet = descriptor.createWriteDescriptorSet();
        writeDescriptorSet.setDstBinding(descriptorDefinition.bindingIndex);
        writeDescriptorSet.setDstSet(m_descriptorSets.at(descriptorDefinition.descriptorSetIndex));

        device.updateDescriptorSets(writeDescriptorSet, nullptr);
    }
    catch (const std::out_of_range &exception)
    {
        std::stringstream exceptionMessage;
        exceptionMessage << "Pipeline contains descriptor '" << identifier << "' but this object does not."
                         << "\n";

        throw std::runtime_error(exceptionMessage.str());
    }
}
} // namespace pvk::engine