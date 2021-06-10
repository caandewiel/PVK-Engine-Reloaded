#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include <iostream>
#include <memory>

#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
constexpr auto validationLayers = {"VK_LAYER_KHRONOS_validation"};

class Instance
{
public:
    Instance();
    ~Instance();

    [[nodiscard]] const vk::Instance &getInstance() const;

private:
    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugUtilsMessenger;
    vk::DispatchLoaderDynamic m_dispatch{};
};

class DebugUtilsMessenger
{
public:
    DebugUtilsMessenger();
    ~DebugUtilsMessenger();

private:
    vk::DebugUtilsMessengerEXT m_debugUtilsMessenger;
    PFN_vkDestroyDebugUtilsMessengerEXT m_destroyFunction;
};
} // namespace pvk::vulkan
#endif // __INSTANCE_H__