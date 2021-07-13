#include "Sampler.hpp"
#include <vulkan/vulkan.hpp>

#include "../engine/Graphics.hpp"

namespace pvk::vulkan
{
Sampler::Sampler()
{
    vk::SamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.setMagFilter(vk::Filter::eLinear);
    samplerCreateInfo.setMinFilter(vk::Filter::eLinear);
    samplerCreateInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAnisotropyEnable(VK_FALSE);
    samplerCreateInfo.setMaxAnisotropy(
        graphics::get()->getDevice().getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy);
    samplerCreateInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
    samplerCreateInfo.setUnnormalizedCoordinates(VK_FALSE);
    samplerCreateInfo.setCompareEnable(VK_FALSE);
    samplerCreateInfo.setCompareOp(vk::CompareOp::eAlways);
    samplerCreateInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
    samplerCreateInfo.setMipLodBias(0.0F);
    samplerCreateInfo.setMinLod(0.0F);
    samplerCreateInfo.setMaxLod(0.0F);

    m_sampler = graphics::get()->getDevice().getLogicalDevice().createSampler(samplerCreateInfo);
}

Sampler::~Sampler()
{
    graphics::get()->getDevice().getLogicalDevice().destroySampler(m_sampler);
}

const vk::Sampler &Sampler::getSampler() const
{
    return m_sampler;
}

} // namespace pvk::vulkan
