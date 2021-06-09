#ifndef RENDERSTAGE_HPP
#define RENDERSTAGE_HPP

#include <filesystem>
#include <memory>

#include "../vulkan/CommandBuffer.hpp"
#include "../vulkan/DescriptorPool.hpp"
#include "../vulkan/Pipeline.hpp"

namespace pvk::engine
{
class RenderStageBase
{
public:
    RenderStageBase() = default;
    RenderStageBase(const pvk::vulkan::RenderPass &renderPass,
                    const std::filesystem::path &pathVertexShader,
                    const std::filesystem::path &pathFragmentShader);
    virtual ~RenderStageBase();

    virtual void render(const command_buffer::CommandBuffer &commandBuffer) const = 0;

protected:
    std::unique_ptr<vulkan::Pipeline> m_pipeline;
    std::unique_ptr<vulkan::DescriptorPool> m_descriptorPool;
};
} // namespace pvk::engine

#endif // RENDERSTAGE_HPP
