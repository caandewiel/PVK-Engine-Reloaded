#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "Device.hpp"
#include "VulkanObject.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace pvk::vulkan
{
class Semaphore: public pvk::vulkan::VulkanObject
{
public:
    Semaphore();
    ~Semaphore();

    [[nodiscard]] const vk::Semaphore &getSemaphore() const;

private:
    vk::Semaphore m_semaphore;
};
} // namespace pvk::vulkan

#endif // __SEMAPHORE_H__