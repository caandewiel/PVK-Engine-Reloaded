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
} // namespace pvk::helper

#endif // HELPER_HPP
