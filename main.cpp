#include <fmt/core.h>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <fstream>

#include "lib/engine/Application.hpp"
#include "lib/engine/Graphics.hpp"
#include "lib/engine/RenderPipeline.hpp"
#include "lib/engine/RenderStageBase.hpp"
#include "lib/geometry/Mesh.hpp"
#include "lib/geometry/Vertex.hpp"
#include "lib/ui/RenderStageUI.hpp"
#include "lib/vulkan/CommandBuffer.hpp"
#include "lib/vulkan/Pipeline.hpp"

class RenderStageMesh : public pvk::engine::RenderStageBase
{
public:
    void init()
    {
        m_mesh = std::make_unique<pvk::geometry::Mesh>("/Users/christian/Beta_Surface.pvm");

        m_pipeline = std::make_unique<pvk::vulkan::Pipeline>(
            pvk::graphics::get()->getRenderPipeline<pvk::engine::RenderPipeline>().getRenderPass(),
            "/Users/christian/CLionProjects/pvk-reloaded/shaders/triangle.vert.spv",
            "/Users/christian/CLionProjects/pvk-reloaded/shaders/triangle.frag.spv");
    }

    void render(const pvk::command_buffer::CommandBuffer &commandBuffer) const override
    {
        auto &cb = commandBuffer.getCommandBuffer(0);
        VkDeviceSize offset = 0;
        cb.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline->getPipeline());
        cb.bindVertexBuffers(0, m_mesh->getVertexBuffer().getBuffer(), offset);
        cb.bindIndexBuffer(m_mesh->getIndexBuffer().getBuffer(), offset, vk::IndexType::eUint32);
        cb.drawIndexed(m_mesh->getNumberOfIndices(), 1, 0, 0, 0);
    }

private:
    std::unique_ptr<pvk::geometry::Mesh> m_mesh;
};

class MyRenderPipeline : public pvk::engine::RenderPipeline
{
public:
    void initialize()
    {
        m_renderStageMesh = std::make_unique<RenderStageMesh>();
        m_renderStageMesh->init();
    }

    void render(const pvk::command_buffer::CommandBuffer &commandBuffer)
    {
        m_renderStageMesh->render(commandBuffer);
    }

private:
    std::unique_ptr<RenderStageMesh> m_renderStageMesh{};
};

class PvkApplication : public pvk::Application
{
public:
    void initialize() override
    {
        pvk::graphics::get()->setRenderPipeline(std::make_unique<MyRenderPipeline>());

        auto &renderPipeline = pvk::graphics::get()->getRenderPipeline<MyRenderPipeline>();

        renderPipeline.initialize();
        renderPipeline.getRenderStageUI().getLogger().addLog("This is a simple test message\n");
    }

    void update() const override
    {
        const auto content =
            fmt::format("Physical Device: {}\nFPS: {}",
                        pvk::graphics::get()->getDevice().getPhysicalDevice().getProperties().deviceName,
                        ImGui::GetIO().Framerate);
        pvk::graphics::get()->getRenderPipeline<MyRenderPipeline>().getRenderStageUI().getOverlayWidget().setContent(
            content);
    }
};

int main()
{
    auto application = std::make_unique<PvkApplication>();
    application->initialize();
    application->run();

    return 0;
}
