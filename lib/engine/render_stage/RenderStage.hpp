#ifndef RENDERSTAGE_HPP
#define RENDERSTAGE_HPP

#include <filesystem>
#include <memory>

#include <absl/container/flat_hash_map.h>

#include <vulkan/vulkan.hpp>

#include "ShaderReflection.hpp"

#include "../shader/Descriptor.hpp"

#include "../../geometry/Object.hpp"

#include "../../vulkan/CommandBuffer.hpp"
#include "../../vulkan/DescriptorPool.hpp"
#include "../../vulkan/Pipeline.hpp"

namespace pvk::engine
{
class RenderStage
{
public:
    RenderStage() = default;
    RenderStage(std::unique_ptr<pvk::vulkan::Pipeline> &&pipeline,
                std::unique_ptr<pvk::vulkan::DescriptorPool> &&descriptorPool,
                absl::flat_hash_map<std::string, pipeline::DescriptorDefinition> &&descriptorDefinitions);

    virtual ~RenderStage();

    void renderObject(const command_buffer::CommandBuffer &commandBuffer, const geometry::Object &object) const;
    void bindDescriptor(const std::string &identifier, const Descriptor &descriptor);

protected:
    std::unique_ptr<vulkan::Pipeline> m_pipeline;
    std::unique_ptr<vulkan::DescriptorPool> m_descriptorPool;

private:
    absl::flat_hash_map<std::string, pipeline::DescriptorDefinition> m_descriptorDefinitions;
    absl::flat_hash_map<std::string, std::unique_ptr<Descriptor>> m_descriptors;
    absl::flat_hash_map<uint8_t, vk::DescriptorSet> m_descriptorSets;
};
} // namespace pvk::engine

#endif // RENDERSTAGE_HPP
