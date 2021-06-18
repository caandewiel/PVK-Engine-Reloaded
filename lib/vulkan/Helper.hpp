#ifndef HELPER_HPP
#define HELPER_HPP

#include <functional>
#include <string>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"

#include "../engine/Graphics.hpp"

namespace pvk::vulkan
{
void assertVulkan(const vk::Result &result, std::string &&message);

template <typename T> T assertVulkan(const vk::ResultValue<T> &result, std::string &&message)
{
    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error(message);
    }
    return result.value;
}

std::unique_ptr<pvk::vulkan::Buffer> createEmptyBuffer(size_t bufferSize,
                                                       vk::BufferUsageFlags bufferUsage,
                                                       VmaMemoryUsage memoryUsage);

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
                                     vk::ImageAspectFlags imageAspectFlags);

template <typename T>
std::unique_ptr<pvk::vulkan::Buffer> createBuffer(std::vector<T> &&data,
                                                  vk::BufferUsageFlags bufferUsage,
                                                  VmaMemoryUsage memoryUsage)
{
    vk::BufferCreateInfo vertexBufferCreateInfo{};
    vertexBufferCreateInfo.setUsage(bufferUsage);
    vertexBufferCreateInfo.setSize(data.size() * sizeof(T));

    VmaAllocationCreateInfo vertexAllocationCreateInfo = {};
    vertexAllocationCreateInfo.usage = memoryUsage;

    auto buffer =
        graphics::get()->getMemoryAllocator().createBuffer(vertexBufferCreateInfo, vertexAllocationCreateInfo);

    void *temporaryData;
    vmaMapMemory(graphics::get()->getMemoryAllocator().getAllocator(), buffer->getAllocation(), &temporaryData);

    memcpy(temporaryData, data.data(), data.size() * sizeof(T));

    vmaUnmapMemory(graphics::get()->getMemoryAllocator().getAllocator(), buffer->getAllocation());

    return buffer;
}

void executeOneTimeCommandBuffer(
    const pvk::vulkan::CommandPool &commandPool,
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function);

void executeOneTimeCommandBuffer(
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function);
} // namespace pvk::vulkan

#endif // HELPER_HPP
