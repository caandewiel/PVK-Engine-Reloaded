#include "RenderStageBase.hpp"
#include "Graphics.hpp"
#include "pipeline/ShaderReflection.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace
{
constexpr uint32_t DESCRIPTOR_POOL_MAX_NUMBER_OF_SETS = 8192;
constexpr uint32_t DESCRIPTOR_COUNT_4096 = 4096;
constexpr uint32_t DESCRIPTOR_COUNT_2048 = 2048;

std::unique_ptr<pvk::vulkan::DescriptorPool> createDescriptorPool()
{
    // This workaround is adapted from:
    // https://github.com/EQMG/Acid/blob/cb1e62a80cdba662a0b2c1ba008b2bf4a397877a/Sources/Graphics/Pipelines/Shader.cpp
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        {vk::DescriptorType::eCombinedImageSampler, DESCRIPTOR_COUNT_4096},
        {vk::DescriptorType::eUniformBuffer, DESCRIPTOR_COUNT_2048},
        {vk::DescriptorType::eStorageImage, DESCRIPTOR_COUNT_2048},
        {vk::DescriptorType::eUniformTexelBuffer, DESCRIPTOR_COUNT_2048},
        {vk::DescriptorType::eStorageTexelBuffer, DESCRIPTOR_COUNT_2048},
        {vk::DescriptorType::eStorageBuffer, DESCRIPTOR_COUNT_2048},
    };

    return std::make_unique<pvk::vulkan::DescriptorPool>(
        descriptorPoolSizes, vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, DESCRIPTOR_POOL_MAX_NUMBER_OF_SETS);
}
} // namespace

namespace pvk::engine
{
RenderStageBase::RenderStageBase(const pvk::vulkan::RenderPass &renderPass,
                                 const std::filesystem::path &pathVertexShader,
                                 const std::filesystem::path &pathFragmentShader)
{
    m_descriptorPool = createDescriptorPool();
    // pipeline::createPipelineDefinitionFromSPIRVShader(pathVertexShader, pathFragmentShader);
}

RenderStageBase::~RenderStageBase()
{
    m_descriptorPool.reset();
    m_pipeline.reset();
}
} // namespace pvk::engine