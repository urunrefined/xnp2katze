#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

namespace BR {

class VulkanImageMemory {
    const VkDevice &device;
    VkDeviceMemory imageMemory;

  public:
    VulkanImageMemory(const VkPhysicalDevice &physicalDevice,
                      const VkDevice &device_, VkImage image,
                      VkMemoryPropertyFlags properties);

    ~VulkanImageMemory();

    operator VkDeviceMemory &() { return imageMemory; }
};

} // namespace BR
