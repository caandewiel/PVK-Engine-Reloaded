#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Device.hpp"
#include "Image.hpp"
#include "Window.hpp"

namespace pvk::vulkan
{
class SwapChain
{
public:
    SwapChain();
    ~SwapChain();

    [[nodiscard]] const vk::SwapchainKHR &getSwapChain() const;

    [[nodiscard]] uint32_t getNumberOfSwapChainImages() const;

    [[nodiscard]] const vk::SurfaceFormatKHR &getSurfaceFormat() const;

    [[nodiscard]] const std::vector<vk::Image> &getSwapChainImages() const;

    [[nodiscard]] const Image &getDepthImage() const;

    [[nodiscard]] const vk::ImageView &getSwapChainImageView(uint8_t index) const;

    [[nodiscard]] const vk::Extent2D &getSwapChainExtent() const;

private:
    vk::SwapchainKHR m_swapChain;
    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::ImageView> m_swapChainImageViews;
    std::unique_ptr<Image> m_depthImage;
    vk::SurfaceFormatKHR m_surfaceFormat;
    vk::Extent2D m_extent;
};
} // namespace pvk::vulkan

#endif // __SWAPCHAINMAKER_H__