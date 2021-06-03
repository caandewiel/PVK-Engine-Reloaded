#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "Instance.hpp"
#include "Surface.hpp"
#include <memory>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
constexpr auto deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class Device
{
public:
    explicit Device(bool enableValidationLayers = true);
    ~Device();
    [[nodiscard]] const vk::PhysicalDevice &getPhysicalDevice() const;
    [[nodiscard]] const vk::Device &getLogicalDevice() const;
    [[nodiscard]] const vk::Queue &getGraphicsQueue() const;
    [[nodiscard]] const vk::Queue &getPresentQueue() const;
    [[nodiscard]] vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates,
                                                 vk::ImageTiling tiling,
                                                 vk::FormatFeatureFlags features) const;
    [[nodiscard]] const QueueFamilyIndices &getQueueFamilyIndices() const;

private:
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_logicalDevice;
    QueueFamilyIndices m_queueFamilyIndices;
    vk::Queue m_graphicsQueue;
    vk::Queue m_presentQueue;
};

QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);
} // namespace pvk::vulkan
#endif // __DEVICE_H__