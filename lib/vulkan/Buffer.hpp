#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace pvk::vulkan
{
class Buffer
{
public:
    Buffer(vk::Buffer &&buffer, VmaAllocation &&allocation);
    ~Buffer();

    [[nodiscard]] const vk::Buffer &getBuffer() const;
    [[nodiscard]] const VmaAllocation &getAllocation() const;
private:
    vk::Buffer m_buffer;
    VmaAllocation m_allocation;
};

// void create();
} // namespace pvk::vulkan

#endif // BUFFER_HPP
