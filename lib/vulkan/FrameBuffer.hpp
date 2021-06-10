#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <vulkan/vulkan.hpp>

#include "RenderPass.hpp"

namespace pvk::vulkan
{
class FrameBuffer
{
public:
    explicit FrameBuffer(const vulkan::RenderPass &renderPass);
    ~FrameBuffer();

    [[nodiscard]] const vk::Framebuffer &getFrameBuffer(uint32_t index) const;

private:
    std::vector<vk::Framebuffer> m_frameBuffers{};
};
} // namespace pvk::vulkan

#endif // __FRAMEBUFFER_H__