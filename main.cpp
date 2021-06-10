#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <fmt/core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "lib/engine/Application.hpp"
#include "lib/engine/Graphics.hpp"
#include "lib/engine/RenderPipeline.hpp"
#include "lib/engine/RenderStage.hpp"
#include "lib/engine/pipeline/RenderStageBuilder.hpp"

#include "lib/geometry/Object.hpp"

#include "lib/ui/TreeWidget.hpp"

#include "lib/vulkan/CommandBuffer.hpp"

#include "lib/io/AssetLoader.hpp"

class MyRenderPipeline : public pvk::engine::RenderPipeline
{
public:
    void initialize()
    {
        auto builder = std::make_unique<pvk::engine::pipeline::RenderStageBuilder>();
        m_renderStage =
            builder->setVertexShader("/Users/christian/CLionProjects/pvk-reloaded/shaders/triangle.vert.spv")
                .setFragmentShader("/Users/christian/CLionProjects/pvk-reloaded/shaders/triangle.frag.spv")
                .setRenderPass(pvk::graphics::get()->getRenderPipeline<pvk::engine::RenderPipeline>().getRenderPass())
                .create();
        m_object = pvk::io::loadObject("/Users/christian/walk/walk.object");

        auto treeWidget = std::make_unique<pvk::ui::TreeWidget>();
        treeWidget->setContent(*m_object);
        registerWidget("objectOverview", std::move(treeWidget));
    }

    void render(const pvk::command_buffer::CommandBuffer &commandBuffer) override
    {
        m_renderStage->renderObject(commandBuffer, *m_object);
    }

private:
    std::unique_ptr<pvk::engine::RenderStage> m_renderStage{};
    std::unique_ptr<pvk::geometry::Object> m_object{};
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
