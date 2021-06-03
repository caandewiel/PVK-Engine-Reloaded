#include "Semaphore.hpp"
#include "../engine/Graphics.hpp"
#include <memory>

namespace pvk::vulkan
{
Semaphore::Semaphore()
{
    this->m_semaphore = graphics::get()->getDevice().getLogicalDevice().createSemaphore({});
}

Semaphore::~Semaphore() 
{
    graphics::get()->getDevice().getLogicalDevice().destroySemaphore(this->m_semaphore);
}

const vk::Semaphore &Semaphore::getSemaphore() const
{
    return this->m_semaphore;
}
} // namespace pvk::vulkan