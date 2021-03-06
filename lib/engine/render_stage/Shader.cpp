#include "Shader.hpp"

#include <memory>
#include <utility>

#include <vulkan/vulkan.hpp>

#include "../Graphics.hpp"
#include "glm/fwd.hpp"

namespace pvk::engine
{
Shader::Shader(std::unique_ptr<pvk::vulkan::Pipeline> &&pipeline,
               absl::flat_hash_map<std::string, pipeline::DescriptorDefinition> &&descriptorDefinitions)
    : m_pipeline(std::move(pipeline)), m_descriptorDefinitions(std::move(descriptorDefinitions))
{
}

Shader::~Shader()
{
    m_pipeline.reset();
}

void Shader::renderEntity(const command_buffer::CommandBuffer &commandBuffer, const Entity &entity) const
{
    const auto &cb = commandBuffer.getCommandBuffer(0);
    cb.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline->getPipeline());
    cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                          m_pipeline->getPipelineLayout(),
                          0,
                          1,
                          &m_globalDescriptorSets.at(0),
                          0,
                          nullptr);

    struct {
        glm::mat4 transform;
        glm::vec3 lightPosition;
    } pushConstant;

    pushConstant.transform = entity.getTransformMatrix();
    pushConstant.lightPosition = entity.getObject().getBounds().first;
    // pushConstant.lightPosition = {10.0F, 0.0F, 0.0F};

    cb.pushConstants(m_pipeline->getPipelineLayout(),
                     vk::ShaderStageFlagBits::eAllGraphics,
                     0,
                     sizeof(glm::mat4) + sizeof(glm::vec3),
                     &pushConstant);

    renderNode(commandBuffer, entity.getObject().getRootNode());
}

void Shader::renderNode(const command_buffer::CommandBuffer &commandBuffer, const geometry::Node &node) const
{
    const auto offset = 0;
    const auto &cb = commandBuffer.getCommandBuffer(0);

    for (const auto &mesh : node.getMeshes())
    {
        cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                              m_pipeline->getPipelineLayout(),
                              1,
                              1,
                              &mesh.lock()->getMaterial().getDescriptorSet(),
                              0,
                              nullptr);
        cb.bindVertexBuffers(0, mesh.lock()->getVertexBuffer().getBuffer(), offset);
        cb.bindIndexBuffer(mesh.lock()->getIndexBuffer().getBuffer(), offset, vk::IndexType::eUint32);
        cb.drawIndexed(mesh.lock()->getNumberOfIndices(), 1, 0, 0, 0);
    }

    for (const auto &child : node.getChildren())
    {
        renderNode(commandBuffer, *child.lock());
    }
}

void Shader::bindDescriptor(const std::string &identifier, const Descriptor &descriptor)
{
    try
    {
        const auto &device = graphics::get()->getDevice().getLogicalDevice();

        auto descriptorDefinition = m_descriptorDefinitions.at(identifier);
        auto descriptorSetLayout = m_pipeline->getDescriptorSetLayout(descriptorDefinition.descriptorSetIndex);
        std::vector<vk::DescriptorSetLayout> layouts(pvk::graphics::get()->getSwapChain().getNumberOfSwapChainImages(),
                                                     descriptorSetLayout);

        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
        descriptorSetAllocateInfo.setDescriptorPool(pvk::graphics::get()->getDescriptorPool().getDescriptorPool());
        descriptorSetAllocateInfo.setSetLayouts(descriptorSetLayout);
        descriptorSetAllocateInfo.setDescriptorSetCount(1);

        auto descriptorSets = device.allocateDescriptorSets(descriptorSetAllocateInfo);
        m_globalDescriptorSets.insert(std::make_pair(descriptorDefinition.descriptorSetIndex, *descriptorSets.begin()));

        auto writeDescriptorSet = descriptor.createWriteDescriptorSet();
        writeDescriptorSet.setDstBinding(descriptorDefinition.bindingIndex);
        writeDescriptorSet.setDstSet(m_globalDescriptorSets.at(descriptorDefinition.descriptorSetIndex));

        device.updateDescriptorSets(writeDescriptorSet, nullptr);
    }
    catch (const std::out_of_range &exception)
    {
        std::stringstream exceptionMessage;
        exceptionMessage << "Pipeline contains descriptor '" << identifier << "' but this object does not."
                         << "\n";

        throw std::runtime_error(exceptionMessage.str());
    }
}

void Shader::bindObjectDescriptor(
    const geometry::Object &object,
    const std::string &identifier,
    const std::function<const Descriptor &(const geometry::Object &object, const geometry::Mesh &mesh)> &callback)
{
    std::vector<const Descriptor *> descriptors{};

    // for (size_t i = 0; i < object.getNumberOfMeshes(); i++)
    // {
    //     descriptors.emplace_back(&callback(object, object.getMesh(i)));
    // }

    const auto &device = graphics::get()->getDevice().getLogicalDevice();

    auto descriptorDefinition = m_descriptorDefinitions.at(identifier);

    for (const auto &[_, mesh] : object.m_meshLookup)
    {
        auto descriptorSetLayout = m_pipeline->getDescriptorSetLayout(descriptorDefinition.descriptorSetIndex);
        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
        descriptorSetAllocateInfo.setDescriptorPool(pvk::graphics::get()->getDescriptorPool().getDescriptorPool());
        descriptorSetAllocateInfo.setSetLayouts(descriptorSetLayout);

        mesh->m_material.lock()->m_descriptorSet = *device.allocateDescriptorSets(descriptorSetAllocateInfo).begin();

        auto writeDescriptorSets = mesh->getMaterial().getAllWriteDescriptorSets();

        device.updateDescriptorSets(writeDescriptorSets, nullptr);
    }
}
} // namespace pvk::engine