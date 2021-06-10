#include <fmt/core.h>

#include "Application.hpp"
#include "Engine.hpp"

namespace pvk
{
Application::Application()
{
    pvk::engine::Engine::initialize();
}

Application::~Application()
{
    pvk::engine::Engine::tearDown();
}

void Application::run()
{
    pvk::engine::Engine::run(*this);
}

void log(const std::string &message)
{
    pvk::graphics::get()->getRenderPipeline<pvk::engine::RenderPipeline>().getRenderStageUI().getLogger().addLog(
        fmt::format("{}\n", message).c_str());
}
} // namespace pvk
