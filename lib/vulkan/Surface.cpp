#include <memory>
#include <type_traits>
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../engine/Graphics.hpp"
#include "Surface.hpp"

namespace pvk::vulkan
{
Surface::Surface()
{
    const auto &instance = graphics::get()->getInstance();
    const auto &window = graphics::get()->getWindow();

    if (glfwCreateWindowSurface(instance.getInstance(), window.getWindow(), nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface.");
    }
}

Surface::~Surface()
{
    graphics::get()->getInstance().getInstance().destroySurfaceKHR(m_surface);
}

const vk::SurfaceKHR Surface::getSurface() const
{
    return m_surface;
}
} // namespace pvk::vulkan