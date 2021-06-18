#ifndef RENDERSTAGEBUILDER_HPP
#define RENDERSTAGEBUILDER_HPP

#include <filesystem>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "RenderStage.hpp"

#include "../../vulkan/DescriptorPool.hpp"
#include "../../vulkan/RenderPass.hpp"

namespace pvk::engine::pipeline
{
class RenderStageBuilder
{
public:
    [[nodiscard]] RenderStageBuilder &setVertexShader(const std::filesystem::path &vertexShader);

    [[nodiscard]] RenderStageBuilder &setFragmentShader(const std::filesystem::path &fragmentShader);

    [[nodiscard]] RenderStageBuilder &setRenderPass(const vulkan::RenderPass &renderPass);

    [[nodiscard]] RenderStageBuilder &setViewport(const vk::Extent2D &viewport);

    [[nodiscard]] std::unique_ptr<pvk::engine::RenderStage> create();

private:
    std::filesystem::path m_pathToVertexShader;
    std::filesystem::path m_pathToFragmentShader;

    vk::Extent2D m_viewport;

    const vulkan::RenderPass* m_renderPass;
};
} // namespace pvk::engine::pipeline

#endif // RENDERSTAGEBUILDER_HPP
