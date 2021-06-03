#include "DescriptorPool.hpp"
#include "../engine/Graphics.hpp"

#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
DescriptorPool::DescriptorPool(const std::vector<vk::DescriptorPoolSize> &poolSizes,
                               vk::DescriptorPoolCreateFlags flags,
                               uint32_t maxSets)
{
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.setPoolSizes(poolSizes);
    descriptorPoolCreateInfo.setFlags(flags);
    descriptorPoolCreateInfo.setMaxSets(maxSets);

    m_descriptorPool = graphics::get()->getDevice().getLogicalDevice().createDescriptorPool(descriptorPoolCreateInfo);
}

DescriptorPool::~DescriptorPool()
{
    graphics::get()->getDevice().getLogicalDevice().destroyDescriptorPool(m_descriptorPool);
}

const vk::DescriptorPool& DescriptorPool::getDescriptorPool() const
{
    return m_descriptorPool;
}
} // namespace pvk::vulkan