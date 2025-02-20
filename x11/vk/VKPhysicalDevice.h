#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace BR {

class VulkanPhysicalDevice {
  public:
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties deviceProperties;
    std::vector<VkQueueFamilyProperties> queueFamilies;

    VulkanPhysicalDevice(VkPhysicalDevice physicalDevice);
    ~VulkanPhysicalDevice() = default;

    bool isDeviceSuitable(VkSurfaceKHR surface);

    VkDeviceSize getMinUniformBufferOffsetAlignment() const {
        return deviceProperties.limits.minUniformBufferOffsetAlignment;
    }

    operator const VkPhysicalDevice &() const { return physicalDevice; }
};

} // namespace BR
