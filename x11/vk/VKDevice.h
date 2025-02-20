#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace BR {

class VulkanDevice {
    VulkanDevice(VulkanDevice &) = delete;

  public:
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    uint32_t graphicsFamily;
    uint32_t presentFamily;

    VulkanDevice(bool enableValidationLayer, VkPhysicalDevice physicalDevice,
                 VkSurfaceKHR surface,
                 const std::vector<VkQueueFamilyProperties> &queueFamilies);
    ~VulkanDevice();

    operator VkDevice &() { return device; }

    operator const VkDevice &() const { return device; }
};

class VulkanPureDevice {
    VulkanPureDevice(VulkanPureDevice &) = delete;

  public:
    VkDevice device;
    VkQueue graphicsQueue;
    uint32_t graphicsFamily;

    VulkanPureDevice(bool enableValidationLayer,
                     VkPhysicalDevice physicalDevice,
                     const std::vector<VkQueueFamilyProperties> &queueFamilies);
    ~VulkanPureDevice();

    operator VkDevice &() { return device; }

    operator const VkDevice &() const { return device; }
};

} // namespace BR
