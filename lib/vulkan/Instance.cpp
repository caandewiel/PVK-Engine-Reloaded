#include <memory>
#include <sstream>
#include <type_traits>

#include <GLFW/glfw3.h>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "Instance.hpp"

#include "../engine/Graphics.hpp"

namespace
{
static std::vector<const char *> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData)
{

    std::string message;
    std::string color;

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        color = "\033[31m";
    }
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        color = "\033[33m";
    }
    else
    {
        color = "\033[30m";
    }

    message += vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) + ": " +
               vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)) + ":\n";
    message += std::string("\t") + "messageIDName   = <" + pCallbackData->pMessageIdName + ">\n";
    message += std::string("\t") + "messageIdNumber = " + std::to_string(pCallbackData->messageIdNumber) + "\n";
    message += std::string("\t") + "message         = <" + pCallbackData->pMessage + ">\n";
    if (0 < pCallbackData->queueLabelCount)
    {
        message += std::string("\t") + "Queue Labels:\n";
        for (uint8_t i = 0; i < pCallbackData->queueLabelCount; i++)
        {
            message += std::string("\t\t") + "labelName = <" + pCallbackData->pQueueLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount)
    {
        message += std::string("\t") + "CommandBuffer Labels:\n";
        for (uint8_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
        {
            message += std::string("\t\t") + "labelName = <" + pCallbackData->pCmdBufLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->objectCount)
    {
        for (uint8_t i = 0; i < pCallbackData->objectCount; i++)
        {
            message += std::string("\t") + "Object " + std::to_string(i) + "\n";
            message += std::string("\t\t") + "objectType   = " +
                       vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) + "\n";
            message += std::string("\t\t") +
                       "objectHandle = " + std::to_string(pCallbackData->pObjects[i].objectHandle) + "\n";
            if (pCallbackData->pObjects[i].pObjectName)
            {
                message += std::string("\t\t") + "objectName   = <" + pCallbackData->pObjects[i].pObjectName + ">\n";
            }
        }
    }

    std::cout << color << message << "\033[0m" << std::endl;

    return VK_FALSE;
}
} // namespace

namespace pvk::vulkan
{
Instance::Instance()
{
    vk::ApplicationInfo applicationInfo;
    applicationInfo.setPApplicationName("PVK Reloaded");
    applicationInfo.setApplicationVersion(1);
    applicationInfo.setPEngineName("PVK Reloaded");
    applicationInfo.setEngineVersion(1);
    applicationInfo.setApiVersion(VK_API_VERSION_1_2);

    const auto extensions = getRequiredExtensions();

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
    debugUtilsMessengerCreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    debugUtilsMessengerCreateInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
    debugUtilsMessengerCreateInfo.setPfnUserCallback(debugCallback);

    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.setFlags(vk::InstanceCreateFlags());
    instanceCreateInfo.setPApplicationInfo(&applicationInfo);
    instanceCreateInfo.setPEnabledExtensionNames(extensions);
    instanceCreateInfo.setPNext(&debugUtilsMessengerCreateInfo);
    instanceCreateInfo.setPEnabledLayerNames(validationLayers);

    vk::DynamicLoader dynamicLoader;
    auto vkGetInstanceProcAddr = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    m_dispatch.init(vkGetInstanceProcAddr);
    m_instance = vk::createInstance(instanceCreateInfo);
    m_dispatch.init(m_instance);
    m_debugUtilsMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo, nullptr, m_dispatch);
}

Instance::~Instance()
{
    m_instance.destroyDebugUtilsMessengerEXT(m_debugUtilsMessenger, nullptr, m_dispatch);
    m_instance.destroy(nullptr, m_dispatch);
}

const vk::Instance &Instance::getInstance() const
{
    return m_instance;
}

DebugUtilsMessenger::DebugUtilsMessenger()
{
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
    debugUtilsMessengerCreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    debugUtilsMessengerCreateInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
    debugUtilsMessengerCreateInfo.setPfnUserCallback(debugCallback);

    const auto &instance = graphics::get()->getInstance().getInstance();
    m_debugUtilsMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo);
    m_destroyFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
}

DebugUtilsMessenger::~DebugUtilsMessenger()
{
    const auto &instance = graphics::get()->getInstance().getInstance();
    m_destroyFunction(instance, m_debugUtilsMessenger, nullptr);
}
} // namespace pvk::vulkan
