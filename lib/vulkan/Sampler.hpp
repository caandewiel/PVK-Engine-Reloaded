#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class Sampler
{
public:
    Sampler();
    ~Sampler();

    [[nodiscard]] const vk::Sampler& getSampler() const;

private:
    vk::Sampler m_sampler;
};
} // namespace pvk::vulkan

#endif // __SAMPLER_H__