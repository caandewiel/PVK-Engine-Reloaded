#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>

#include "../vulkan/Device.hpp"
#include "Application.hpp"
#include "Graphics.hpp"

namespace pvk::engine
{
class Engine
{
public:
    static void initialize();
    static void run(Application &application);
    static void tearDown();
};
} // namespace pvk::engine

#endif // ENGINE_HPP
