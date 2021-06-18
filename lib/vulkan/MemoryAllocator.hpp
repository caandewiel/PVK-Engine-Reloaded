#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include <memory>
#include <vulkan/vulkan.hpp>

#include "Buffer.hpp"
#include "Device.hpp"
#include "Image.hpp"
#include "Instance.hpp"

#include "../geometry/Mesh.hpp"

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
    [[nodiscard]] std::unique_ptr<Image> createImage(vk::ImageCreateInfo &imageCreateInfo,
                                                     vk::ImageViewCreateInfo &imageViewCreateInfo,
                                                     VmaAllocationCreateInfo &allocationCreateInfo) const;

private:
    std::unique_ptr<VmaAllocator> m_allocator;
};
} // namespace pvk::vulkan

#endif // MEMORYALLOCATOR_HPP
