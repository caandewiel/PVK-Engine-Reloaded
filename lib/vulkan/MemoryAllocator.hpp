#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../../external/proxy/vk_mem_alloc.h"
#include "../geometry/Mesh.hpp"
#include "Buffer.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class MemoryAllocator
{
public:
    MemoryAllocator();
    ~MemoryAllocator();
    [[nodiscard]] const VmaAllocator &getAllocator() const;
    [[nodiscard]] std::unique_ptr<Buffer> createBuffer(const pvk::geometry::Mesh &mesh) const;
    [[nodiscard]] std::unique_ptr<Buffer> createBuffer(const vk::BufferCreateInfo &bufferCreateInfo,
                                         const VmaAllocationCreateInfo &allocationCreateInfo) const;

private:
    std::unique_ptr<VmaAllocator> m_allocator;
};
} // namespace pvk::vulkan

#endif // MEMORYALLOCATOR_HPP
