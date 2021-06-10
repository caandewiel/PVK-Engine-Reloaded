#ifndef RENDERSTAGE_HPP
#define RENDERSTAGE_HPP

#include <filesystem>
#include <memory>

#include "../geometry/Object.hpp"

#include "../vulkan/CommandBuffer.hpp"
#include "../vulkan/DescriptorPool.hpp"
#include "../vulkan/Pipeline.hpp"

namespace pvk::engine
{
class RenderStage
{
public:
    RenderStage() = default;
    explicit RenderStage(std::unique_ptr<RenderStage> &&renderStage);
    RenderStage(std::unique_ptr<pvk::vulkan::Pipeline> &&pipeline,
                std::unique_ptr<pvk::vulkan::DescriptorPool> &&descriptorPool);
    RenderStage(const pvk::vulkan::RenderPass &renderPass,
                const std::filesystem::path &pathVertexShader,
                const std::filesystem::path &pathFragmentShader);

    virtual ~RenderStage();

    void renderObject(const command_buffer::CommandBuffer &commandBuffer, const geometry::Object &object) const;

protected:
    std::unique_ptr<vulkan::Pipeline> m_pipeline;
    std::unique_ptr<vulkan::DescriptorPool> m_descriptorPool;
};
} // namespace pvk::engine

#endif // RENDERSTAGE_HPP
