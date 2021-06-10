#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "Instance.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class Surface
{
public:
    Surface();
    ~Surface();

    [[nodiscard]] const vk::SurfaceKHR getSurface() const;

private:
    VkSurfaceKHR m_surface;
};
} // namespace pvk::vulkan
#endif // __SURFACE_H__