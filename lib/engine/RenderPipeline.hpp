#ifndef RENDERPIPELINE_HPP
#define RENDERPIPELINE_HPP

#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include <absl/container/flat_hash_map.h>

#include <vulkan/vulkan.hpp>

#include "camera/Camera.hpp"
#include "render_stage/Shader.hpp"

#include "../geometry/Scene.hpp"

#include "../vulkan/CommandBuffer.hpp"
#include "../vulkan/CommandPool.hpp"
#include "../vulkan/Fence.hpp"
#include "../vulkan/FrameBuffer.hpp"
#include "../vulkan/Pipeline.hpp"
#include "../vulkan/RenderPass.hpp"
#include "../vulkan/Semaphore.hpp"

#include "../ui/RenderStageUI.hpp"

namespace pvk::engine
{
class RenderPipeline
{
public:
    RenderPipeline();
    virtual ~RenderPipeline();

    virtual void render(const pvk::command_buffer::CommandBuffer &commandBuffer) = 0;
    void render();
    void registerRenderStage(const std::string &identifier, std::unique_ptr<Shader> &&renderStage);

    template <class T> void initializeRenderStage(const std::string &identifier)
    {
        std::unique_ptr<Shader> renderStage = std::make_unique<T>(*m_commandPool, *m_renderPass);
        m_renderStages.insert(std::make_pair(identifier, std::move(renderStage)));
    }

    template <typename T> const T &getRenderStage(const std::string &identifier)
    {
        return dynamic_cast<const T &>(*m_renderStages.at(identifier));
    }

    [[nodiscard]] const vulkan::RenderPass &getRenderPass() const;
    [[nodiscard]] const vulkan::CommandPool &getCommandPool() const;
    [[nodiscard]] const command_buffer::CommandBuffer &getCommandBuffer() const;
    [[nodiscard]] const vulkan::FrameBuffer &getFrameBuffer() const;
    [[nodiscard]] const vulkan::Pipeline &getPipeline() const;
    [[nodiscard]] const ui::RenderStageUI &getRenderStageUI() const;

    void log(const std::string &content) const;

    void registerWidget(const std::string &identifier, std::unique_ptr<ui::Widget> &&widget);

    void setScene(std::weak_ptr<geometry::Scene> scene);
    void setCamera(std::weak_ptr<engine::Camera> camera);

    [[nodiscard]] const geometry::Scene &getScene() const;
    [[nodiscard]] const engine::Camera &getCamera() const;
    [[nodiscard]] engine::Camera &getCamera();

private:
    std::unique_ptr<vulkan::RenderPass> m_renderPass{};
    std::unique_ptr<vulkan::CommandPool> m_commandPool{};
    std::unique_ptr<command_buffer::CommandBuffer> m_commandBuffer{};
    std::unique_ptr<vulkan::FrameBuffer> m_frameBuffer{};
    std::unique_ptr<vulkan::Pipeline> m_pipeline{};
    std::unique_ptr<vulkan::Fence> m_renderFence{};
    std::unique_ptr<vulkan::Semaphore> m_presentSemaphore{};
    std::unique_ptr<vulkan::Semaphore> m_renderSemaphore{};
    std::unique_ptr<ui::RenderStageUI> m_renderStageUI{};

    absl::flat_hash_map<std::string, std::unique_ptr<Shader>> m_renderStages{};

    std::weak_ptr<geometry::Scene> m_scene{};
    std::weak_ptr<engine::Camera> m_camera{};

    [[nodiscard]] uint32_t getNextImageFromSwapChain();
    void submitCommandBufferToGraphicsQueue();
    void presentGraphicsQueue(uint32_t imageIndex);
};
} // namespace pvk::engine

#endif // RENDERPIPELINE_HPP
