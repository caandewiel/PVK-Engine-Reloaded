#ifndef HELPER_HPP
#define HELPER_HPP

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include <functional>
#include <string>
#include <vulkan/vulkan.hpp>

namespace pvk::helper
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

void executeOneTimeCommandBuffer(
    const pvk::vulkan::CommandPool &commandPool,
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function);

void executeOneTimeCommandBuffer(
    std::function<void(const pvk::command_buffer::CommandBuffer &commandBuffer)> &&function);
} // namespace pvk::helper

#endif // HELPER_HPP
