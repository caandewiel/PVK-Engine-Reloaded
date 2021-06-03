#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "../../external/proxy/vk_mem_alloc.h"
#include "../vulkan/Device.hpp"
#include "Application.hpp"
#include "Graphics.hpp"
#include <memory>

namespace pvk::engine
{
class Engine
{
public:
    static void initialize();
    static void run(const Application &application);
    static void tearDown();
};
} // namespace pvk::engine

#endif // ENGINE_HPP