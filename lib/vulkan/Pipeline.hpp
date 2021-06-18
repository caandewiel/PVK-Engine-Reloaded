#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Device.hpp"
#include "RenderPass.hpp"

namespace pvk::vulkan
{
class Pipeline
{
public:
    Pipeline(const vk::Pipeline &pipeline,
             const vk::PipelineLayout &pipelineLayout,
             std::vector<vk::ShaderModule> &&shaderModules,
             std::vector<vk::DescriptorSetLayout> &&descriptorSetLayouts);
    ~Pipeline();

    [[nodiscard]] const vk::DescriptorSetLayout &getDescriptorSetLayout(uint32_t index) const;
    [[nodiscard]] const vk::Pipeline &getPipeline() const;
    [[nodiscard]] const vk::PipelineLayout &getPipelineLayout() const;

private:
    vk::Pipeline m_pipeline;
    vk::PipelineLayout m_pipelineLayout;
    std::vector<vk::ShaderModule> m_shaderModules;
    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
};
} // namespace pvk::vulkan

#endif // PIPELINE_HPP
