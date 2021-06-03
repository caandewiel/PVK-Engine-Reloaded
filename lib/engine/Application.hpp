#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "../vulkan/CommandBuffer.hpp"
#include <functional>

namespace pvk
{
class Application
{
public:
    Application();
    virtual ~Application();
    virtual void initialize() = 0;
    virtual void update() const = 0;
    void run();
};

void log(const std::string &message);
} // namespace pvk

#endif // __APPLICATION_H__