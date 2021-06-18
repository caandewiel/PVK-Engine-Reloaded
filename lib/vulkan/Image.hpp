#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace pvk::vulkan
{
class Image
{
public:
    Image(const vk::Image &image, const vk::ImageView &imageView, const VmaAllocation &allocation);
    ~Image();

    [[nodiscard]] const vk::Image &getImage() const;
    [[nodiscard]] const vk::ImageView &getImageView() const;
    [[nodiscard]] const VmaAllocation &getAllocation() const;
private:
    vk::Image m_image;
    vk::ImageView m_imageView;
    VmaAllocation m_allocation;
};
} // namespace pvk::vulkan

#endif // __IMAGE_H__