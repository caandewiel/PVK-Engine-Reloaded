#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "../../external/proxy/vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

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
