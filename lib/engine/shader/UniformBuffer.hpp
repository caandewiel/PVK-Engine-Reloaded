#ifndef UNIFORMBUFFER_HPP
#define UNIFORMBUFFER_HPP

#include "Descriptor.hpp"

#include "../../vulkan/Buffer.hpp"

namespace pvk::engine
{
class UniformBuffer : public Descriptor
{
public:
    explicit UniformBuffer(size_t bufferSize);

    void update(void *data);

    [[nodiscard]] vk::WriteDescriptorSet createWriteDescriptorSet() const override;

private:
    size_t m_bufferSize;

    vk::DescriptorBufferInfo m_descriptorBufferInfo {};
    std::unique_ptr<vulkan::Buffer> m_buffer;
};
} // namespace pvk::engine

#endif // UNIFORMBUFFER_HPP
