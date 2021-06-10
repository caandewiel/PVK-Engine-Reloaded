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
    Pipeline(const vulkan::RenderPass &renderPass, const std::string &vertexShader, const std::string &fragmentShader);
    Pipeline(const vk::Pipeline &pipeline, const vk::PipelineLayout &pipelineLayout, std::vector<vk::ShaderModule> &&shaderModules);
    ~Pipeline();

    [[nodiscard]] const vk::Pipeline &getPipeline() const;

private:
    vk::Pipeline m_pipeline;
    vk::PipelineLayout m_pipelineLayout;
    std::vector<vk::ShaderModule> m_shaderModules;
};
} // namespace pvk::vulkan

#endif // PIPELINE_HPP
