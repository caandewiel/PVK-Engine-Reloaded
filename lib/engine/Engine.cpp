#include "Engine.hpp"

#include <memory>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "Application.hpp"
#include "Graphics.hpp"

namespace pvk::engine
{
void Engine::initialize()
{
    graphics::get()->initialize();
}

void Engine::run(const Application &application)
{
    graphics::get()->render(application);
}

void Engine::tearDown()
{
    graphics::get()->destroy();
}
} // namespace pvk::engine
