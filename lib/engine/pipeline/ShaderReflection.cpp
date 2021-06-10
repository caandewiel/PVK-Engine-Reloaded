#include "ShaderReflection.hpp"

#include <filesystem>
#include <fstream>

#include <spirv_glsl.hpp>

#include "../../asset/AssetHelper.hpp"

namespace
{
std::vector<char> readFile(const std::filesystem::path &path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file.");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

std::vector<uint32_t> loadSPIRVShader(const std::filesystem::path &path)
{
    return pvk::asset::convertBinaryToVector<uint32_t>(readFile(path));
}

std::vector<pvk::engine::pipeline::DescriptorDefinition> getAllUniformBufferDefinitionFromCompiler(
    const spirv_cross::CompilerGLSL &glsl,
    const vk::ShaderStageFlags &shaderStageFlags)
{
    std::vector<pvk::engine::pipeline::DescriptorDefinition> allDefinition;

    for (const auto &uniformBuffer : glsl.get_shader_resources().uniform_buffers)
    {
        pvk::engine::pipeline::UniformBufferDefinition definition{};
        definition.identifier = glsl.get_name(uniformBuffer.id);
        definition.descriptorSetIndex = glsl.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
        definition.bindingIndex = glsl.get_decoration(uniformBuffer.id, spv::DecorationBinding);
        definition.bufferSize = glsl.get_declared_struct_size(glsl.get_type(uniformBuffer.base_type_id));
        definition.descriptorType = vk::DescriptorType::eUniformBuffer;
        definition.shaderStage = vk::ShaderStageFlagBits::eAllGraphics;
        //            definition.shaderStage = shaderStageFlags;

        allDefinition.emplace_back(definition);
    }

    return allDefinition;
}

std::vector<pvk::engine::pipeline::DescriptorDefinition> getAllCombinedImageSamplerDefinitionFromCompiler(
    const spirv_cross::CompilerGLSL &glsl,
    const vk::ShaderStageFlags &shaderStageFlags)
{
    std::vector<pvk::engine::pipeline::DescriptorDefinition> allDefinition;

    for (const auto &sampler : glsl.get_shader_resources().sampled_images)
    {
        pvk::engine::pipeline::CombinedImageSamplerDefinition definition{};
        definition.identifier = glsl.get_name(sampler.id);
        definition.descriptorSetIndex = glsl.get_decoration(sampler.id, spv::DecorationDescriptorSet);
        definition.bindingIndex = glsl.get_decoration(sampler.id, spv::DecorationBinding);
        definition.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        definition.shaderStage = vk::ShaderStageFlagBits::eFragment;

        allDefinition.emplace_back(definition);
    }

    return allDefinition;
}

std::optional<pvk::engine::pipeline::PushConstantDefinition> getPushConstantDefinitionFromCompiler(
    const spirv_cross::CompilerGLSL &glsl,
    const vk::ShaderStageFlags &shaderStageFlags)
{
    const auto &pushConstantsBuffers = glsl.get_shader_resources().push_constant_buffers;

    if (pushConstantsBuffers.empty())
    {
        return std::optional<pvk::engine::pipeline::PushConstantDefinition>();
    }

    if (pushConstantsBuffers.size() > 1)
    {
        std::stringstream exceptionMessage;
        exceptionMessage << "Only one push constant is supported per shader stage, found "
                         << pushConstantsBuffers.size();
        throw std::runtime_error(exceptionMessage.str());
    }

    const auto &pushConstant = *pushConstantsBuffers.begin();

    pvk::engine::pipeline::PushConstantDefinition definition{};
    definition.identifier = glsl.get_name(pushConstant.id);
    definition.bufferSize = glsl.get_declared_struct_size(glsl.get_type(pushConstant.base_type_id));
    definition.shaderStage = vk::ShaderStageFlagBits::eAllGraphics;

    return std::optional<pvk::engine::pipeline::PushConstantDefinition>(definition);
}

std::vector<pvk::engine::pipeline::DescriptorDefinition> getAllDescriptorDefinitionFromCompiler(
    const spirv_cross::CompilerGLSL &glsl,
    const vk::ShaderStageFlags &shaderStageFlags)
{
    auto uniformBufferDefinitions = getAllUniformBufferDefinitionFromCompiler(glsl, shaderStageFlags);
    auto combinedImageSamplerDefinitions = getAllCombinedImageSamplerDefinitionFromCompiler(glsl, shaderStageFlags);

    std::vector<pvk::engine::pipeline::DescriptorDefinition> allDefinition;
    allDefinition.insert(allDefinition.end(), uniformBufferDefinitions.begin(), uniformBufferDefinitions.end());
    allDefinition.insert(
        allDefinition.end(), combinedImageSamplerDefinitions.begin(), combinedImageSamplerDefinitions.end());

    return allDefinition;
}

pvk::engine::pipeline::PipelineDefinition addDescriptorSetLayoutBinding(
    const spirv_cross::CompilerGLSL &glsl,
    const vk::ShaderStageFlags &shaderStageFlags,
    pvk::engine::pipeline::PipelineDefinition &&pipelineDefinition)
{
    auto uniformBufferDefinitions = getAllDescriptorDefinitionFromCompiler(glsl, shaderStageFlags);
    auto pushConstantDefinition = getPushConstantDefinitionFromCompiler(glsl, shaderStageFlags);

    for (const auto &definition : uniformBufferDefinitions)
    {
        std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings{};
        pipelineDefinition.descriptorSets.emplace(
            std::make_pair(definition.descriptorSetIndex, descriptorSetLayoutBindings));
        pipelineDefinition.descriptorDefinitions.emplace(definition.identifier, definition);

        auto &descriptorSet = pipelineDefinition.descriptorSets.at(definition.descriptorSetIndex);
        descriptorSet.emplace_back(definition.bindingIndex, definition.descriptorType, 1, definition.shaderStage);
    }

    if (pushConstantDefinition.has_value())
    {
        pipelineDefinition.pushConstantDefinitions.emplace(
            std::make_pair(pushConstantDefinition.value().identifier, pushConstantDefinition.value()));
    }

    return std::move(pipelineDefinition);
}
} // namespace

namespace pvk::engine::pipeline
{
PipelineDefinition createPipelineDefinitionFromSPIRVShader(const std::string &pathToVertexShader,
                                                           const std::string &pathToFragmentShader)
{
    PipelineDefinition pipelineDefinition;
    pipelineDefinition.vertexShader = pathToVertexShader;
    pipelineDefinition.fragmentShader = pathToFragmentShader;

    spirv_cross::CompilerGLSL glslVertex(loadSPIRVShader(pathToVertexShader));
    spirv_cross::CompilerGLSL glslFragment(loadSPIRVShader(pathToFragmentShader));

    pipelineDefinition =
        addDescriptorSetLayoutBinding(glslVertex, vk::ShaderStageFlagBits::eVertex, std::move(pipelineDefinition));
    pipelineDefinition =
        addDescriptorSetLayoutBinding(glslFragment, vk::ShaderStageFlagBits::eFragment, std::move(pipelineDefinition));

    return pipelineDefinition;
}
} // namespace pvk::engine::pipeline