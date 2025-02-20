#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanImageMemory {
    VkDevice device;
    VkDeviceMemory imageMemory;

  public:
    VulkanImageMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                      VkImage image, VkMemoryPropertyFlags properties);

    ~VulkanImageMemory();

    operator VkDeviceMemory &() { return imageMemory; }
};

} // namespace BR
