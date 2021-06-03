#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "Device.hpp"
#include "RenderPass.hpp"
#include "VulkanObject.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class Pipeline : public vulkan::VulkanObject
{
public:
    Pipeline(const vulkan::RenderPass &renderPass,
             const std::string &vertexShader,
             const std::string &fragmentShader);
    ~Pipeline();

    [[nodiscard]] const vk::Pipeline &getPipeline() const;

private:
    vk::Pipeline m_pipeline;
    vk::PipelineLayout m_pipelineLayout;
    std::vector<vk::ShaderModule> m_shaderModules;
};
} // namespace pvk::vulkan

#endif // PIPELINE_HPP
