#include "Descriptor.hpp"

namespace pvk::engine
{
const vk::DescriptorSet &Descriptor::getDescriptorSet(uint32_t index) const
{
    return m_descriptorSets.at(index);
}
} // namespace pvk::engine