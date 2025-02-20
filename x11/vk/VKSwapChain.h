#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanSwapChain {
    VkDevice device;
    VkSwapchainKHR swapChain;

  public:
    VkFormat format;
    VkExtent2D extent;

    operator VkSwapchainKHR() { return swapChain; }

    VulkanSwapChain(VkDevice device, VkPhysicalDevice physicalDevice,
                    uint32_t graphicsFamily, uint32_t presentFamily,
                    VkSurfaceKHR surface, uint32_t width, uint32_t height,
                    VulkanSwapChain *oldSwapChain);
    ~VulkanSwapChain();
};

} // namespace BR
