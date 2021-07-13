#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <vulkan/vulkan.hpp>
#include "Descriptor.hpp"

#include "../../vulkan/Image.hpp"
#include "../../vulkan/Sampler.hpp"

namespace pvk::engine
{
class Texture : public Descriptor
{
public:
    explicit Texture(size_t bufferSize, uint32_t width, uint32_t height);

    void update(void *data);

    [[nodiscard]] vk::WriteDescriptorSet createWriteDescriptorSet() const override;

private:
    size_t m_bufferSize;
    uint32_t m_width;
    uint32_t m_height;

    vk::DescriptorImageInfo m_descriptorImageInfo{};
    std::unique_ptr<vulkan::Image> m_image;
    std::unique_ptr<vulkan::Sampler> m_sampler;
};
} // namespace pvk::engine

#endif // __TEXTURE_H__