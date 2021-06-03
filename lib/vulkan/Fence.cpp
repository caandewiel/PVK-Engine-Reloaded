#include "Fence.hpp"
#include "../engine/Graphics.hpp"
#include <iterator>
#include <memory>
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
Fence::Fence(vk::FenceCreateFlags flags)
{
    vk::FenceCreateInfo createInfo {};
    createInfo.setFlags(flags);

    this->m_fence = graphics::get()->getDevice().getLogicalDevice().createFence(createInfo);
}

Fence::~Fence()
{
    graphics::get()->getDevice().getLogicalDevice().destroyFence(m_fence);
}

const vk::Fence &Fence::getFence() const
{
    return m_fence;
}
} // namespace pvk::vulkan