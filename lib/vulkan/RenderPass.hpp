#ifndef __RENDERPASSMAKER_H__
#define __RENDERPASSMAKER_H__

#include "Device.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    [[nodiscard]] const vk::RenderPass &getRenderPass() const;

private:
    vk::RenderPass m_renderPass;
};
} // namespace pvk::vulkan

#endif // __RENDERPASSMAKER_H__