#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <functional>

#include "../vulkan/CommandBuffer.hpp"

namespace pvk
{
class Application
{
public:
    Application();
    virtual ~Application();
    virtual void initialize() = 0;
    virtual void update() = 0;
    void run();
};

void log(const std::string &message);
} // namespace pvk

#endif // __APPLICATION_H__