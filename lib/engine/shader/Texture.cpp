#include "Texture.hpp"
#include <memory>

#include <vulkan/vulkan.hpp>

#include "../../vulkan/Helper.hpp"

namespace pvk::engine
{
Texture::Texture(size_t bufferSize, uint32_t width, uint32_t height)
    : m_bufferSize(bufferSize), m_width(width), m_height(height)
{
    m_image = vulkan::createImage2D(m_width,
                                    m_height,
                                    1,
                                    1,
                                    vk::SampleCountFlagBits::e1,
                                    vk::Format::eR8G8B8A8Srgb,
                                    vk::ImageTiling::eOptimal,
                                    vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
                                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                                    {},
                                    vk::ImageAspectFlagBits::eColor);

    m_sampler = std::make_unique<vulkan::Sampler>();

    m_descriptorImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    m_descriptorImageInfo.setImageView(m_image->getImageView());
    m_descriptorImageInfo.setSampler(m_sampler->getSampler());
}

void Texture::update(void *data)
{
    auto buffer =
        vulkan::createEmptyBuffer(m_bufferSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);

    void *temporaryData;
    vmaMapMemory(graphics::get()->getMemoryAllocator().getAllocator(), buffer->getAllocation(), &temporaryData);

    memcpy(temporaryData, data, m_bufferSize);

    vmaUnmapMemory(graphics::get()->getMemoryAllocator().getAllocator(), buffer->getAllocation());

    const auto &bufferRef = buffer->getBuffer();

    vulkan::executeOneTimeCommandBuffer([this, &bufferRef](const pvk::command_buffer::CommandBuffer &commandBuffer) {
        vk::ImageSubresourceRange range;
        range.setAspectMask(vk::ImageAspectFlagBits::eColor);
        range.setBaseMipLevel(0);
        range.setLevelCount(1);
        range.setBaseArrayLayer(0);
        range.setLayerCount(1);

        vk::ImageMemoryBarrier barrierWrite;
        barrierWrite.setOldLayout(vk::ImageLayout::eUndefined);
        barrierWrite.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
        barrierWrite.setImage(m_image->getImage());
        barrierWrite.setSubresourceRange(range);
        barrierWrite.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        commandBuffer.getCommandBuffer(0).pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                                          vk::PipelineStageFlagBits::eTransfer,
                                                          {},
                                                          nullptr,
                                                          nullptr,
                                                          barrierWrite);

        vk::BufferImageCopy copyRegion;
        copyRegion.setBufferOffset(0);
        copyRegion.setBufferRowLength(0);
        copyRegion.setBufferImageHeight(0);
        copyRegion.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1});
        copyRegion.setImageExtent({m_width, m_height, 1});

        commandBuffer.getCommandBuffer(0).copyBufferToImage(
            bufferRef, m_image->getImage(), vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);

        vk::ImageMemoryBarrier barrierRead;
        barrierRead.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
        barrierRead.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        barrierRead.setImage(m_image->getImage());
        barrierRead.setSubresourceRange(range);
        barrierRead.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrierRead.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        commandBuffer.getCommandBuffer(0).pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                          vk::PipelineStageFlagBits::eFragmentShader,
                                                          {},
                                                          nullptr,
                                                          nullptr,
                                                          barrierRead);
    });
}

vk::WriteDescriptorSet Texture::createWriteDescriptorSet() const
{
    vk::WriteDescriptorSet writeDescriptorSet;
    writeDescriptorSet.setImageInfo(m_descriptorImageInfo);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);

    return writeDescriptorSet;
}
} // namespace pvk::engine