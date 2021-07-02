#ifndef RENDERSTAGEBUILDER_HPP
#define RENDERSTAGEBUILDER_HPP

#include <filesystem>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "Shader.hpp"

#include "../../vulkan/DescriptorPool.hpp"
#include "../../vulkan/RenderPass.hpp"

namespace pvk::engine::pipeline
{
class ShaderBuilder
{
public:
    [[nodiscard]] ShaderBuilder &setVertexShader(const std::filesystem::path &vertexShader);

    [[nodiscard]] ShaderBuilder &setFragmentShader(const std::filesystem::path &fragmentShader);

    [[nodiscard]] ShaderBuilder &setRenderPass(const vulkan::RenderPass &renderPass);

    [[nodiscard]] ShaderBuilder &setViewport(const vk::Extent2D &viewport);

    [[nodiscard]] std::unique_ptr<pvk::engine::Shader> create();

private:
    std::filesystem::path m_pathToVertexShader;
    std::filesystem::path m_pathToFragmentShader;

    vk::Extent2D m_viewport;

    const vulkan::RenderPass* m_renderPass;
};
} // namespace pvk::engine::pipeline

#endif // RENDERSTAGEBUILDER_HPP
