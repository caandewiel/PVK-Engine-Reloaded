#ifndef __FENCE_H__
#define __FENCE_H__

#include "Device.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class Fence
{
public:
    explicit Fence(vk::FenceCreateFlags flags = vk::FenceCreateFlagBits::eSignaled);
    ~Fence();

    [[nodiscard]] const vk::Fence &getFence() const;

private:
    vk::Fence m_fence;
};
} // namespace pvk::vulkan

#endif // __FENCE_H__