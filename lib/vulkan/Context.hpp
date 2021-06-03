#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <vulkan/vulkan.hpp>

namespace pvk {
    class Context {
    public:
        static void tearDown();

        static void setPhysicalDevice(vk::PhysicalDevice &&_physicalDevice);

        static void setLogicalDevice(vk::UniqueDevice &&_logicalDevice);

        static void setCommandPool(vk::UniqueCommandPool &&_commandPool);

        static void setPipelineCache(vk::UniquePipelineCache &&_pipelineCache);

        static void setInstance(vk::UniqueInstance &&_instance);

        static void setGraphicsQueue(vk::Queue &&_queue);

        static void setSwapChainImages(std::vector<vk::Image> _swapChainImages);
        
        static vk::PhysicalDevice getPhysicalDevice();

        static vk::Device getLogicalDevice();

        static vk::CommandPool getCommandPool();

        static vk::PipelineCache getPipelineCache();

        static vk::Queue getGraphicsQueue();

        static vk::Instance getInstance();

        static const std::vector<vk::Image> &getSwapChainImages();

        static size_t getNumberOfSwapChainImages();

    private:
        Context() = default;

    };
}

#endif // __CONTEXT_H__