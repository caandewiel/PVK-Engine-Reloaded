#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "../vulkan/CommandBuffer.hpp"

namespace pvk::geometry
{
class Drawable
{
public:
    virtual ~Drawable() = default;
    virtual void draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const = 0;
};
} // namespace pvk::geometry

#endif // DRAWABLE_HPP
