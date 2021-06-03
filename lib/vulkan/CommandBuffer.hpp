#ifndef __COMMANDBUFFER_H__
#define __COMMANDBUFFER_H__

#include "CommandPool.hpp"
#include "Device.hpp"
#include <vulkan/vulkan.hpp>

namespace pvk::command_buffer
{
class CommandBuffer
{
public:
    explicit CommandBuffer(const vulkan::CommandPool &commandPool);

    // Command buffers are automatically destroyed on command pool destruction.
    ~CommandBuffer() = default;

    [[nodiscard]] const vk::CommandBuffer &getCommandBuffer(uint32_t index) const;

private:
    std::vector<vk::CommandBuffer> m_commandBuffers;
};

std::unique_ptr<CommandBuffer> create(const vulkan::Device &device,
                                      const vulkan::CommandPool &commandPool);
} // namespace pvk::command_buffer

#endif // __COMMANDBUFFER_H__