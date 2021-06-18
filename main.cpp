#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>

#include <fmt/core.h>
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "glm/ext/matrix_clip_space.hpp"
#include "lib/engine/Application.hpp"
#include "lib/engine/Graphics.hpp"
#include "lib/engine/RenderPipeline.hpp"
#include "lib/engine/camera/Camera.hpp"
#include "lib/engine/render_stage/RenderStage.hpp"
#include "lib/engine/render_stage/RenderStageBuilder.hpp"
#include "lib/engine/shader/UniformBuffer.hpp"

#include "lib/geometry/Object.hpp"

#include "lib/ui/TreeWidget.hpp"

#include "lib/vulkan/CommandBuffer.hpp"

#include "lib/io/AssetLoader.hpp"

struct UniformBufferObject
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model;
    glm::vec3 color;
};

class MyRenderPipeline : public pvk::engine::RenderPipeline
{
public:
    void initialize()
    {
        pvk::engine::pipeline::RenderStageBuilder builder{};
        m_renderStage =
            builder.setVertexShader("/Users/christian/CLionProjects/pvk-reloaded/shaders/triangle.vert.spv")
                .setFragmentShader("/Users/christian/CLionProjects/pvk-reloaded/shaders/triangle.frag.spv")
                .setRenderPass(pvk::graphics::get()->getRenderPipeline<pvk::engine::RenderPipeline>().getRenderPass())
                .create();

        auto swapChainExtent = pvk::graphics::get()->getSwapChain().getSwapChainExtent();
        m_camera = std::make_unique<pvk::engine::Camera>(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F));

        m_object = pvk::io::loadObject("/Users/christian/walk2/walk2.object");
        m_object->pushConstant("transform", &pushConstant);

        auto bounds = m_object->getBounds();
        auto center = (bounds.first + bounds.second) / glm::vec3(2.0F);
        auto radius = center - bounds.first - bounds.first;

        ubo.model = glm::mat4(1.0f);
        ubo.view = glm::lookAt(radius, center, glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.projection =
            glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
        ubo.projection[1][1] *= -1;
        ubo.color = {1.0F, 0.0F, 1.0F};

        m_bufferObject = std::make_unique<pvk::engine::UniformBuffer>(sizeof(UniformBufferObject));
        m_bufferObject->update(&ubo);
        m_renderStage->bindDescriptor("uniformBufferObject", *m_bufferObject);

        auto treeWidget = std::make_unique<pvk::ui::TreeWidget>();
        treeWidget->setContent(*m_object);
        registerWidget("objectOverview", std::move(treeWidget));

        glfwSetCursorPosCallback(pvk::graphics::get()->getWindow().getWindow(), handleMouseInput);
        glfwSetKeyCallback(pvk::graphics::get()->getWindow().getWindow(), handleKeyBoardInput);
    }

    static void handleKeyBoardInput(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        auto *app = reinterpret_cast<MyRenderPipeline *>(glfwGetWindowUserPointer(window));

        switch (key)
        {
        case GLFW_KEY_W:
            if (action == GLFW_PRESS)
            {
                app->wPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                app->wPressed = false;
            }
            break;

        case GLFW_KEY_A:
            if (action == GLFW_PRESS)
            {
                app->aPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                app->aPressed = false;
            }
            break;

        case GLFW_KEY_S:
            if (action == GLFW_PRESS)
            {
                app->sPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                app->sPressed = false;
            }
            break;

        case GLFW_KEY_D:
            if (action == GLFW_PRESS)
            {
                app->dPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                app->dPressed = false;
            }
            break;

        default:
            break;
        }
    }

    static void handleMouseInput(GLFWwindow *window, double mouseX, double mouseY)
    {
        auto *app = reinterpret_cast<MyRenderPipeline *>(glfwGetWindowUserPointer(window));

        if (app->initializeMouse)
        {
            app->lastMouseX = static_cast<float>(mouseX);
            app->lastMouseY = static_cast<float>(mouseY);
            app->initializeMouse = false;
        }
        else
        {
            app->xOffset = mouseX - app->lastMouseX;
            app->yOffset = app->lastMouseY - mouseY;

            app->lastMouseX = mouseX;
            app->lastMouseY = mouseY;

            app->isMouseActive = true;
        }
    }

    void update()
    {
        auto deltaTime = 0.0001F;

        if (this->wPressed)
        {
            m_camera->update(pvk::engine::FORWARD, deltaTime);
        }
        if (this->aPressed)
        {
            m_camera->update(pvk::engine::LEFT, deltaTime);
        }
        if (sPressed)
        {
            m_camera->update(pvk::engine::BACKWARD, deltaTime);
        }
        if (dPressed)
        {
            m_camera->update(pvk::engine::RIGHT, deltaTime);
        }

        if (this->isMouseActive)
        {
            m_camera->update(static_cast<float>(xOffset), static_cast<float>(yOffset));
            this->isMouseActive = false;
        }
        else
        {
            this->xOffset = 0;
            this->yOffset = 0;
        }

        ubo.view = m_camera->getViewMatrix();
        // m_bufferObject->update(&ubo);
    }

    void render(const pvk::command_buffer::CommandBuffer &commandBuffer) override
    {
        m_renderStage->renderObject(commandBuffer, *m_object);
    }

private:
    std::unique_ptr<pvk::engine::RenderStage> m_renderStage{};
    std::unique_ptr<pvk::geometry::Object> m_object{};
    std::unique_ptr<pvk::engine::Camera> m_camera{};
    std::unique_ptr<pvk::engine::UniformBuffer> m_bufferObject{};

    UniformBufferObject ubo{};

    glm::vec4 pushConstant{1.0F, 1.0F, 0.0F, 0.0F};

    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;

    bool initializeMouse = true;
    bool isMouseActive = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    double xOffset = 0.0;
    double yOffset = 0.0;
};

class PvkApplication : public pvk::Application
{
public:
    void initialize() override
    {
        pvk::graphics::get()->setRenderPipeline(std::make_unique<MyRenderPipeline>());

        auto &renderPipeline = pvk::graphics::get()->getRenderPipeline<MyRenderPipeline>();
        glfwSetWindowUserPointer(pvk::graphics::get()->getWindow().getWindow(), &renderPipeline);
        renderPipeline.initialize();
        renderPipeline.log("This is a simple test message\n");
    }

    void update() const override
    {
        const auto content =
            fmt::format("Physical Device: {}\nFPS: {}",
                        pvk::graphics::get()->getDevice().getPhysicalDevice().getProperties().deviceName,
                        ImGui::GetIO().Framerate);
        pvk::graphics::get()->getRenderPipeline<MyRenderPipeline>().getRenderStageUI().getOverlayWidget().setContent(
            content);

        pvk::graphics::get()->getRenderPipeline<MyRenderPipeline>().update();
    }
};

int main()
{
    auto application = std::make_unique<PvkApplication>();
    application->initialize();
    application->run();

    return 0;
}
