#ifndef DESCRIPTORPOOL_HPP
#define DESCRIPTORPOOL_HPP

#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class DescriptorPool
{
public:
    DescriptorPool(const std::vector<vk::DescriptorPoolSize> &poolSizes,
                   vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                   uint32_t maxSets = 1000);
    ~DescriptorPool();

    [[nodiscard]] const vk::DescriptorPool &getDescriptorPool() const;

private:
    vk::DescriptorPool m_descriptorPool;
};
} // namespace pvk::vulkan

#endif // DESCRIPTORPOOL_HPP
