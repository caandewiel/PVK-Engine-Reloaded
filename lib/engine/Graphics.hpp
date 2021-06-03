#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../vulkan/CommandBuffer.hpp"
#include "../vulkan/CommandPool.hpp"
#include "../vulkan/Context.hpp"
#include "../vulkan/Device.hpp"
#include "../vulkan/Fence.hpp"
#include "../vulkan/FrameBuffer.hpp"
#include "../vulkan/Instance.hpp"
#include "../vulkan/MemoryAllocator.hpp"
#include "../vulkan/Pipeline.hpp"
#include "../vulkan/RenderPass.hpp"
#include "../vulkan/Semaphore.hpp"
#include "../vulkan/Surface.hpp"
#include "../vulkan/SwapChain.hpp"
#include "../vulkan/Window.hpp"
#include "Application.hpp"
#include "RenderPipeline.hpp"
#include <deque>
#include <functional>
#include <iterator>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::graphics
{
class Graphics
{
public:
    Graphics() = default;
    Graphics(const Graphics &other) = delete;
    Graphics(Graphics &&other) = default;

    Graphics &operator=(const Graphics &other) = delete;
    Graphics &operator=(Graphics &&other) = default;

    ~Graphics() = default;

    static Graphics *_get();

    void initialize();
    void render(const Application &application) const;
    void destroy();

    [[nodiscard]] const vulkan::Window &getWindow() const;
    [[nodiscard]] const vulkan::Instance &getInstance() const;
    [[nodiscard]] const vulkan::Device &getDevice() const;
    [[nodiscard]] const vulkan::Surface &getSurface() const;
    [[nodiscard]] const vulkan::SwapChain &getSwapChain() const;
    [[nodiscard]] const vulkan::MemoryAllocator &getMemoryAllocator() const;

    template<typename T>
    [[nodiscard]] const engine::RenderPipeline &getRenderPipeline() const {
        return dynamic_cast<const T &>(*m_renderPipeline);
    };

    template<typename T>
    [[nodiscard]] T &getRenderPipeline() {
        return dynamic_cast<T &>(*m_renderPipeline);
    };

    void setRenderPipeline(std::unique_ptr<engine::RenderPipeline> &&renderPipeline);

private:
    void draw() const;

    static std::unique_ptr<Graphics> m_graphics;

    std::unique_ptr<vulkan::Window> m_window{};
    std::unique_ptr<vulkan::Instance> m_instance{};
    std::unique_ptr<vulkan::Surface> m_surface{};
    std::unique_ptr<vulkan::Device> m_device{};
    std::unique_ptr<vulkan::SwapChain> m_swapChain{};
    std::unique_ptr<engine::RenderPipeline> m_renderPipeline{};
    std::unique_ptr<vulkan::MemoryAllocator> m_memoryAllocator{};
};

Graphics *get();
} // namespace pvk::graphics

#endif // GRAPHICS_HPP
