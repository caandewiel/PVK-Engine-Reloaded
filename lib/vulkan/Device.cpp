#include "Device.hpp"

#include <set>
#include <stdexcept>

#include "../engine/Graphics.hpp"

namespace
{
bool checkDeviceExtensionSupport(const vk::PhysicalDevice &device, const std::vector<const char *> &deviceExtensions)
{
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : device.enumerateDeviceExtensionProperties())
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

pvk::vulkan::SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device,
                                                           const vk::SurfaceKHR &surface)
{
    pvk::vulkan::SwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}

bool isPhysicalDeviceSuitable(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
{
    const auto indices = pvk::vulkan::findQueueFamilies(device, surface);
    const auto extensionsSupported = checkDeviceExtensionSupport(device, pvk::vulkan::deviceExtensions);
    auto swapChainAdequate = false;

    if (extensionsSupported)
    {
        const auto swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

vk::PhysicalDevice determinePhysicalDevice(const vk::Instance &instance,
                                           const vk::SurfaceKHR &surface,
                                           const std::vector<const char *> &deviceExtensions)
{
    const auto devices = instance.enumeratePhysicalDevices();

    for (const auto &device : devices)
    {
        if (isPhysicalDeviceSuitable(device, surface))
        {
            return device;
        }
    }

    throw std::runtime_error("Could not find suitable Vulkan device.");
}

vk::Device createLogicalDevice(const vk::PhysicalDevice &physicalDevice,
                               const vk::SurfaceKHR &surface,
                               const bool enableValidationLayers)
{
    const auto indices = pvk::vulkan::findQueueFamilies(physicalDevice, surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    const auto queuePriority = 1.0F;

    queueCreateInfos.reserve(uniqueQueueFamilies.size());

    for (const auto queueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(),
                                      queueFamily,
                                      1, // queueCount
                                      &queuePriority);
    }

    const auto deviceFeatures = vk::PhysicalDeviceFeatures();
    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setFlags(vk::DeviceCreateFlags());
    deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);
    deviceCreateInfo.setPEnabledExtensionNames(pvk::vulkan::deviceExtensions);
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);

    if (enableValidationLayers)
    {
        deviceCreateInfo.setPEnabledLayerNames(pvk::vulkan::validationLayers);
    }

    try
    {
        return physicalDevice.createDevice(deviceCreateInfo);
    }
    catch (vk::SystemError &error)
    {
        throw std::runtime_error("Could not create logical device.");
    }
}
} // namespace

namespace pvk::vulkan
{
Device::Device(bool enableValidationLayers)
{
    this->m_physicalDevice = determinePhysicalDevice(
        graphics::get()->getInstance().getInstance(), graphics::get()->getSurface().getSurface(), deviceExtensions);
    this->m_logicalDevice =
        createLogicalDevice(this->m_physicalDevice, graphics::get()->getSurface().getSurface(), enableValidationLayers);
    this->m_queueFamilyIndices = findQueueFamilies(this->m_physicalDevice, graphics::get()->getSurface().getSurface());

    m_graphicsQueue = m_logicalDevice.getQueue(this->m_queueFamilyIndices.graphicsFamily.value(), 0);
    m_presentQueue = m_logicalDevice.getQueue(this->m_queueFamilyIndices.presentFamily.value(), 0);
}

Device::~Device()
{
    this->m_logicalDevice.destroy();
}

const vk::PhysicalDevice &Device::getPhysicalDevice() const
{
    return m_physicalDevice;
}

const vk::Device &Device::getLogicalDevice() const
{
    return m_logicalDevice;
}

const vk::Queue &Device::getGraphicsQueue() const
{
    return m_graphicsQueue;
}

const vk::Queue &Device::getPresentQueue() const
{
    return m_graphicsQueue;
}

vk::Format Device::findSupportedFormat(const std::vector<vk::Format> &candidates,
                                       vk::ImageTiling tiling,
                                       vk::FormatFeatureFlags features) const
{
    for (vk::Format format : candidates)
    {
        vk::FormatProperties properties = m_physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }

        if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format");
}

const QueueFamilyIndices &Device::getQueueFamilyIndices() const
{
    return m_queueFamilyIndices;
}

QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface)
{
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();

    uint32_t i = 0;

    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        if (queueFamily.queueCount > 0 && (device.getSurfaceSupportKHR(i, surface) != 0U))
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}
} // namespace pvk::vulkan