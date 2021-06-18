#include "UniformBuffer.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

#include "../../vulkan/Helper.hpp"

namespace pvk::engine
{
UniformBuffer::UniformBuffer(size_t bufferSize) : m_bufferSize(bufferSize)
{
    m_buffer = pvk::vulkan::createEmptyBuffer(
        bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU);
    m_descriptorBufferInfo.setBuffer(m_buffer->getBuffer());
    m_descriptorBufferInfo.setOffset(0);
    m_descriptorBufferInfo.setRange(m_bufferSize);
}

void UniformBuffer::update(void *data)
{
    void *temporaryData;
    vmaMapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_buffer->getAllocation(), &temporaryData);

    memcpy(temporaryData, data, m_bufferSize);

    vmaUnmapMemory(graphics::get()->getMemoryAllocator().getAllocator(), m_buffer->getAllocation());
}

vk::WriteDescriptorSet UniformBuffer::createWriteDescriptorSet() const
{
    vk::WriteDescriptorSet writeDescriptorSet;
    writeDescriptorSet.setBufferInfo(m_descriptorBufferInfo);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);

    return writeDescriptorSet;
}
} // namespace pvk::engine