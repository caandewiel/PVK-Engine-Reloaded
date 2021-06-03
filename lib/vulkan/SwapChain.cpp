#include "SwapChain.hpp"
#include "../engine/Graphics.hpp"
#include "Device.hpp"
#include "Surface.hpp"
#include "Window.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace
{
struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
{
    SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
    {
        return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
    }

    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
{
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }

        if (availablePresentMode == vk::PresentModeKHR::eImmediate)
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

vk::Extent2D chooseSwapExtent(const pvk::vulkan::Window &window, const vk::SurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(window.getWindow(), &width, &height);

    vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actualExtent.width =
        std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height =
        std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

uint32_t getNumberOfImages(const SwapChainSupportDetails &swapChainSupportDetails)
{
    auto imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

    if (swapChainSupportDetails.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupportDetails.capabilities.maxImageCount)
    {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    return imageCount;
}

std::vector<vk::ImageView> createSwapChainImageViews(const pvk::vulkan::Device &device,
                                                     const std::vector<vk::Image> &swapChainImages,
                                                     const vk::Format &format)
{
    std::vector<vk::ImageView> swapChainImageViews;

    for (const auto &swapChainImage : swapChainImages)
    {
        vk::ImageViewCreateInfo createInfo;
        createInfo.setImage(swapChainImage);
        createInfo.setViewType(vk::ImageViewType::e2D);
        createInfo.setFormat(format);
        createInfo.setComponents({vk::ComponentSwizzle::eIdentity,
                                  vk::ComponentSwizzle::eIdentity,
                                  vk::ComponentSwizzle::eIdentity,
                                  vk::ComponentSwizzle::eIdentity});
        createInfo.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        try
        {
            swapChainImageViews.emplace_back(device.getLogicalDevice().createImageView(createInfo));
        }
        catch (vk::SystemError &error)
        {
            throw std::runtime_error("Failed to create image views.");
        }
    }

    return swapChainImageViews;
}
} // namespace

namespace pvk::vulkan
{
SwapChain::SwapChain()
{
    const auto &device = graphics::get()->getDevice();
    const auto &surface = graphics::get()->getSurface();
    const auto &window = graphics::get()->getWindow();

    const auto swapChainSupportDetails = querySwapChainSupport(device.getPhysicalDevice(), surface.getSurface());
    const auto presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);

    m_surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    m_extent = chooseSwapExtent(window, swapChainSupportDetails.capabilities);

    vk::SwapchainCreateInfoKHR createInfo(vk::SwapchainCreateFlagsKHR(),
                                          surface.getSurface(),
                                          getNumberOfImages(swapChainSupportDetails),
                                          m_surfaceFormat.format,
                                          m_surfaceFormat.colorSpace,
                                          m_extent,
                                          1, // imageArrayLayers
                                          vk::ImageUsageFlagBits::eColorAttachment);

    const auto indices = pvk::vulkan::findQueueFamilies(device.getPhysicalDevice(), surface.getSurface());
    std::array<uint32_t, 2> queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        createInfo.setQueueFamilyIndices(queueFamilyIndices);
    }
    else
    {
        createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    }

    createInfo.setPreTransform(swapChainSupportDetails.capabilities.currentTransform);
    createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    createInfo.setPresentMode(presentMode);
    createInfo.setClipped(VK_TRUE);
    createInfo.setOldSwapchain(vk::SwapchainKHR(nullptr));

    try
    {
        m_swapChain = device.getLogicalDevice().createSwapchainKHR(createInfo);
        m_swapChainImages = device.getLogicalDevice().getSwapchainImagesKHR(m_swapChain);
        m_swapChainImageViews = createSwapChainImageViews(device, m_swapChainImages, m_surfaceFormat.format);
    }
    catch (vk::SystemError &error)
    {
        throw std::runtime_error("Failed to create swapchain.");
    }
}

SwapChain::~SwapChain()
{
    const auto &logicalDevice = graphics::get()->getDevice().getLogicalDevice();

    std::for_each(m_swapChainImageViews.begin(), m_swapChainImageViews.end(), [&logicalDevice](auto &imageView) {
        logicalDevice.destroyImageView(imageView);
    });

    // SwapChain images are automatically destroyed when destroying the SwapChain.
    logicalDevice.destroySwapchainKHR(m_swapChain);
}

const vk::SwapchainKHR &SwapChain::getSwapChain() const
{
    return m_swapChain;
}

uint32_t SwapChain::getNumberOfSwapChainImages() const
{
    return static_cast<uint32_t>(m_swapChainImages.size());
}

const vk::SurfaceFormatKHR &SwapChain::getSurfaceFormat() const
{
    return m_surfaceFormat;
}

const std::vector<vk::Image> &SwapChain::getSwapChainImages() const
{
    return m_swapChainImages;
}

const vk::ImageView &SwapChain::getSwapChainImageView(uint8_t index) const
{
    try
    {
        return m_swapChainImageViews.at(index);
    }
    catch (std::out_of_range &exception)
    {
        throw std::runtime_error("Invalid SwapChainImageView index.");
    }
}

const vk::Extent2D &SwapChain::getSwapChainExtent() const
{
    return m_extent;
}
} // namespace pvk::vulkan