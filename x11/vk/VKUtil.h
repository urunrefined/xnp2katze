#pragma once

#include "VKInstance.h"
#include "VKPhysicalDevice.h"
#include "VKPhysicalDeviceEnumerations.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace BR {

class SwapChainSupportDetails {
  public:
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    SwapChainSupportDetails(const VkPhysicalDevice device,
                            const VkSurfaceKHR surface);
};

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
VkRect2D getIntegerScissor(unsigned int multipleX, unsigned int multipleY,
                           unsigned int width, unsigned int height);
VkRect2D getAspectScissor(double ideal, unsigned int width,
                          unsigned int height);
VkRect2D get1to1Scissor(unsigned int width, unsigned int height);

bool hasGraphicsQueue(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    const std::vector<VkQueueFamilyProperties> &queueFamilies);

bool hasPresentQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                     const std::vector<VkQueueFamilyProperties> &queueFamilies);

uint32_t
getFirstPresentQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                     const std::vector<VkQueueFamilyProperties> &queueFamilies);

bool getFirstGraphicsQueue(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    const std::vector<VkQueueFamilyProperties> &queueFamilies);

bool getFirstGraphicsQueue(
    const std::vector<VkQueueFamilyProperties> &queueFamilies);

VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanInstance &instance,
                                               VkSurfaceKHR surface);
VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanInstance &instance);
} // namespace BR
