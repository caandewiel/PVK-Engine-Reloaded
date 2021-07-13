#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include <filesystem>
#include <functional>
#include <memory>

#include <absl/container/flat_hash_map.h>

#include <vulkan/vulkan.hpp>

#include "ShaderReflection.hpp"

#include "../entity/Entity.hpp"
#include "../shader/Descriptor.hpp"

#include "../../geometry/Object.hpp"

#include "../../vulkan/CommandBuffer.hpp"
#include "../../vulkan/DescriptorPool.hpp"
#include "../../vulkan/Pipeline.hpp"

namespace pvk::engine
{
class Shader
{
public:
    Shader() = default;
    Shader(std::unique_ptr<pvk::vulkan::Pipeline> &&pipeline,
           absl::flat_hash_map<std::string, pipeline::DescriptorDefinition> &&descriptorDefinitions);

    virtual ~Shader();

    void renderEntity(const command_buffer::CommandBuffer &commandBuffer, const Entity &entity) const;
    void renderNode(const command_buffer::CommandBuffer &commandBuffer, const geometry::Node &node) const;
    void bindDescriptor(const std::string &identifier, const Descriptor &descriptor);
    void bindObjectDescriptor(
        const geometry::Object &object,
        const std::string &identifier,
        const std::function<const Descriptor &(const geometry::Object &object, const geometry::Mesh &mesh)> &callback);

protected:
    std::unique_ptr<vulkan::Pipeline> m_pipeline;

private:
    absl::flat_hash_map<std::string, pipeline::DescriptorDefinition> m_descriptorDefinitions;
    absl::flat_hash_map<std::string, std::unique_ptr<Descriptor>> m_descriptors;
    absl::flat_hash_map<uint8_t, vk::DescriptorSet> m_globalDescriptorSets;
    absl::flat_hash_map<std::pair<const geometry::Mesh *, uint8_t>, vk::DescriptorSet> m_meshDescriptorSets;
};
} // namespace pvk::engine

#endif // ENGINE_SHADER_HPP
