#ifndef RENDERSTAGE_HPP
#define RENDERSTAGE_HPP

#include "../vulkan/DescriptorPool.hpp"
#include "../vulkan/Pipeline.hpp"
#include "../vulkan/CommandBuffer.hpp"
#include <memory>

namespace pvk::engine
{
class RenderStageBase
{
public:
    RenderStageBase();
    virtual ~RenderStageBase();

    virtual void render(const command_buffer::CommandBuffer &commandBuffer) const = 0;
protected:
    std::unique_ptr<vulkan::DescriptorPool> m_descriptorPool;
    std::unique_ptr<vulkan::Pipeline> m_pipeline;
};
} // namespace pvk::engine

#endif // RENDERSTAGE_HPP
