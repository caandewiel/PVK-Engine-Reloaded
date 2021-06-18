#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include <vulkan/vulkan.hpp>

namespace pvk::engine
{
class Descriptor
{
public:
    Descriptor() = default;

    Descriptor(const Descriptor &other) = delete;
    Descriptor(Descriptor &&other) = default;

    Descriptor &operator=(const Descriptor &other) = delete;
    Descriptor &operator=(Descriptor &&other) = default;

    virtual ~Descriptor() = default;

    [[nodiscard]] virtual vk::WriteDescriptorSet createWriteDescriptorSet() const = 0;

    [[nodiscard]] const vk::DescriptorSet &getDescriptorSet(uint32_t index) const;

private:
    std::vector<vk::DescriptorSet> m_descriptorSets;
};
} // namespace pvk::engine

#endif // DESCRIPTOR_HPP
