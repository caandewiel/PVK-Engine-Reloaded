#include "Graphics.hpp"

#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include <fmt/core.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "../ui/RenderStageUI.hpp"
#include "../vulkan/Helper.hpp"
#include "Engine.hpp"
#include "RenderPipeline.hpp"
#include "exceptions/PvkExceptionGraphics.hpp"

namespace
{
std::unique_ptr<pvk::vulkan::DescriptorPool> initializeDescriptorPool()
{
    // This workaround is adapted from:
    // https://github.com/EQMG/Acid/blob/cb1e62a80cdba662a0b2c1ba008b2bf4a397877a/Sources/Graphics/Pipelines/Shader.cpp
    constexpr uint32_t descriptorPoolMaxNumberOfSets = 8192;
    constexpr uint32_t descriptorCountHigh = 4096;
    constexpr uint32_t descriptorCountLow = 2048;

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        {vk::DescriptorType::eCombinedImageSampler, descriptorCountHigh},
        {vk::DescriptorType::eUniformBuffer, descriptorCountLow},
        {vk::DescriptorType::eStorageImage, descriptorCountLow},
        {vk::DescriptorType::eUniformTexelBuffer, descriptorCountLow},
        {vk::DescriptorType::eStorageTexelBuffer, descriptorCountLow},
        {vk::DescriptorType::eStorageBuffer, descriptorCountLow},
    };

    return std::make_unique<pvk::vulkan::DescriptorPool>(
        descriptorPoolSizes, vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, descriptorPoolMaxNumberOfSets);
}
} // namespace

namespace pvk::graphics
{
std::unique_ptr<Graphics> Graphics::m_graphics = nullptr;

Graphics *Graphics::_get()
{
    if (m_graphics == nullptr)
    {
        m_graphics = std::make_unique<Graphics>();

        return m_graphics.get();
    }

    return m_graphics.get();
}

void Graphics::initialize()
{
    constexpr uint32_t width = 1280;
    constexpr uint32_t height = 720;
    constexpr auto windowTitle = "PVK Reloaded";

    m_window = pvk::vulkan::create(width, height, windowTitle);
    m_instance = std::make_unique<pvk::vulkan::Instance>();
    m_surface = std::make_unique<pvk::vulkan::Surface>();
    m_device = std::make_unique<pvk::vulkan::Device>(true);
    m_memoryAllocator = std::make_unique<pvk::vulkan::MemoryAllocator>();
    m_swapChain = std::make_unique<pvk::vulkan::SwapChain>();
    m_descriptorPool = initializeDescriptorPool();
}

void Graphics::draw() const
{
    if (m_renderPipeline == nullptr)
    {
        throw PvkExceptionGraphics("Cannot start rendering stage when no render pipeline is defined.");
    }

    m_renderPipeline->render();
}

void Graphics::render(Application &application) const
{
    while (glfwWindowShouldClose(m_window->getWindow()) == 0)
    {
        application.update();
        glfwPollEvents();
        draw();
    }

    m_device->getLogicalDevice().waitIdle();
}

void Graphics::destroy()
{
    m_renderPipeline.reset();
    m_descriptorPool.reset();
    m_swapChain.reset();
    m_surface.reset();
    m_memoryAllocator.reset();
    m_device.reset();
    m_instance.reset();
}

const vulkan::Window &Graphics::getWindow() const
{
    if (m_window == nullptr)
    {
        throw PvkExceptionGraphics("Window has not yet been initialized.");
    }

    return *m_window;
}

const vulkan::Instance &Graphics::getInstance() const
{
    if (m_instance == nullptr)
    {
        throw PvkExceptionGraphics("Instance has not yet been initialized.");
    }

    return *m_instance;
}

const vulkan::Device &Graphics::getDevice() const
{
    if (m_device == nullptr)
    {
        throw PvkExceptionGraphics("Device has not yet been initialized.");
    }

    return *m_device;
}

const vulkan::Surface &Graphics::getSurface() const
{
    if (m_surface == nullptr)
    {
        throw PvkExceptionGraphics("Surface has not yet been initialized.");
    }

    return *m_surface;
}

const vulkan::SwapChain &Graphics::getSwapChain() const
{
    if (m_swapChain == nullptr)
    {
        throw PvkExceptionGraphics("Swapchain has not yet been initialized.");
    }

    return *m_swapChain;
}

const vulkan::DescriptorPool &Graphics::getDescriptorPool() const
{
    if (m_descriptorPool == nullptr)
    {
        throw PvkExceptionGraphics("DescriptorPool has not yet been initialized.");
    }

    return *m_descriptorPool;
}

const vulkan::MemoryAllocator &Graphics::getMemoryAllocator() const
{
    if (m_memoryAllocator == nullptr)
    {
        throw PvkExceptionGraphics("Memory allocator has not yet been initialized.");
    }

    return *m_memoryAllocator;
}

const engine::RenderPipeline& Graphics::getRenderPipeline() const
{
    if (m_renderPipeline == nullptr)
    {
        throw PvkExceptionGraphics("Render pipeline has not yet been initialized.");
    }

    return *m_renderPipeline;
}

void Graphics::setRenderPipeline(std::unique_ptr<engine::RenderPipeline> &&renderPipeline)
{
    m_renderPipeline = std::move(renderPipeline);
}

Graphics *get()
{
    return Graphics::_get();
}
} // namespace pvk::graphics