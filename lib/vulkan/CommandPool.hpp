#ifndef __COMMANDPOOL_H__
#define __COMMANDPOOL_H__

#include "Device.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class CommandPool
{
public:
    CommandPool();
    ~CommandPool();

    [[nodiscard]] const vk::CommandPool &getCommandPool() const;

private:
    vk::CommandPool m_commandPool;
};
} // namespace pvk::vulkan

#endif // __COMMANDPOOL_H__