#include "Image.hpp"

#include "../engine/Graphics.hpp"

namespace pvk::vulkan
{

Image::Image(const vk::Image &image, const vk::ImageView &imageView, const VmaAllocation &allocation)
    : m_image(image), m_imageView(imageView), m_allocation(allocation)
{
}

Image::~Image()
{
    vmaDestroyImage(graphics::get()->getMemoryAllocator().getAllocator(), m_image, m_allocation);
    graphics::get()->getDevice().getLogicalDevice().destroyImageView(m_imageView);
}

const vk::Image &Image::getImage() const
{
    return m_image;
}

const vk::ImageView &Image::getImageView() const
{
    return m_imageView;
}

const VmaAllocation &Image::getAllocation() const
{
    return m_allocation;
}

} // namespace pvk::vulkan
