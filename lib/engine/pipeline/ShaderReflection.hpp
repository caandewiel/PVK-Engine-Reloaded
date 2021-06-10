#ifndef SHADERREFLECTION_HPP
#define SHADERREFLECTION_HPP

#include <filesystem>
#include <string>

#include <absl/container/flat_hash_map.h>

#include <vulkan/vulkan.hpp>

namespace pvk::engine::pipeline
{
struct DescriptorDefinition
{
    std::string identifier;
    uint8_t descriptorSetIndex;
    uint8_t bindingIndex;
    vk::DescriptorType descriptorType;
    vk::ShaderStageFlags shaderStage;
};

struct UniformBufferDefinition : public DescriptorDefinition
{
    size_t bufferSize = 0;
};

struct CombinedImageSamplerDefinition : public DescriptorDefinition
{
};

struct PushConstantDefinition
{
    std::string identifier;
    size_t bufferSize = 0;
    vk::ShaderStageFlags shaderStage;
};

struct PipelineDefinition
{
    std::string vertexShader;
    std::string fragmentShader;
    absl::flat_hash_map<uint8_t, std::vector<vk::DescriptorSetLayoutBinding>> descriptorSets;
    absl::flat_hash_map<std::string, DescriptorDefinition> descriptorDefinitions;
    absl::flat_hash_map<std::string, PushConstantDefinition> pushConstantDefinitions;
};

PipelineDefinition createPipelineDefinitionFromSPIRVShader(const std::string &pathToVertexShader,
                                                           const std::string &pathToFragmentShader);
} // namespace pvk::engine::pipeline

#endif // SHADERREFLECTION_HPP
