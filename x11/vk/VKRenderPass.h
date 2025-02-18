#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanRenderPass {
  public:
    const VkDevice &device;
    VkRenderPass renderPass;

    VulkanRenderPass(const VkDevice &device_,
                     const VkPhysicalDevice &physicalDevice,
                     VkFormat swapChainImageFormat);
    ~VulkanRenderPass();

    operator VkRenderPass &() { return renderPass; }
};

} // namespace BR

