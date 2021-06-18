#include "Helper.hpp"
#include <vulkan/vulkan_core.h>

#include "Fence.hpp"

namespace pvk::vulkan
{
void assertVulkan(const vk::Result &result, std::string &&message)
{
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error(message);
    }
}

std::unique_ptr<pvk::vulkan::Buffer> createEmptyBuffer(size_t bufferSize,
                                                       vk::BufferUsageFlags bufferUsage,
                                                       VmaMemoryUsage memoryUsage)
{
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setUsage(bufferUsage);
    bufferCreateInfo.setSize(bufferSize);

    VmaAllocationCreateInfo vertexAllocationCreateInfo = {};
    vertexAllocationCreateInfo.usage = memoryUsage;

    return graphics::get()->getMemoryAllocator().createBuffer(bufferCreateInfo, vertexAllocationCreateInfo);
}

std::unique_ptr<Image> createImage2D(uint32_t width,
                                     uint32_t height,
                                     uint32_t mipLevels,
                                     uint32_t arrayLayers,
                                     vk::SampleCountFlagBits numSamples,
                                     vk::Format format,
                                     vk::ImageTiling tiling,
                                     vk::ImageUsageFlags usage,
                                     vk::MemoryPropertyFlags properties,
                                     vk::ImageCreateFlags imageCreateFlags,
                                     vk::ImageAspectFlags imageAspectFlags)
{
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.setFlags(imageCreateFlags);
    imageCreateInfo.setImageType(vk::ImageType::e2D);
    imageCreateInfo.setFormat(format);
    imageCreateInfo.setExtent({width, height, 1});
    imageCreateInfo.setMipLevels(mipLevels);
    imageCreateInfo.setArrayLayers(arrayLayers);
    imageCreateInfo.setSamples(numSamples);
    imageCreateInfo.setTiling(tiling);
    imageCreateInfo.setUsage(usage);
    imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

    vk::ImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setFormat(format);
    imageViewCreateInfo.setSubresourceRange({imageAspectFlags, 0, 1, 0, 1});

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(properties);

    return graphics::get()->getMemoryAllocator().createImage(
        imageCreateInfo, imageViewCreateInfo, allocationCreateInfo);
}

void executeOneTimeCommandBuffer(
    const pvk::vulkan::CommandPool &commandPool,
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function)
{
    auto commandBuffer = std::make_unique<command_buffer::CommandBuffer>(commandPool);
    commandBuffer->getCommandBuffer(0).begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    function(*commandBuffer);

    commandBuffer->getCommandBuffer(0).end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer->getCommandBuffer(0));

    graphics::get()->getDevice().getGraphicsQueue().submit(submitInfo, nullptr);
    graphics::get()->getDevice().getLogicalDevice().waitIdle();
}

void executeOneTimeCommandBuffer(
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function)
{
    auto commandBuffer = std::make_unique<command_buffer::CommandBuffer>(
        graphics::get()->getRenderPipeline<pvk::engine::RenderPipeline>().getCommandPool());
    commandBuffer->getCommandBuffer(0).begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    function(*commandBuffer);

    commandBuffer->getCommandBuffer(0).end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer->getCommandBuffer(0));

    graphics::get()->getDevice().getGraphicsQueue().submit(submitInfo, nullptr);
    graphics::get()->getDevice().getLogicalDevice().waitIdle();
}
} // namespace pvk::vulkan