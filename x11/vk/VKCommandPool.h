#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanCommandPool {
    const VkDevice device;

  public:
    VkCommandPool commandPool;

    VulkanCommandPool(VkDevice device, uint32_t graphicsFamily);
    VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;
    VulkanCommandPool(const VulkanCommandPool &) = delete;

    ~VulkanCommandPool();

    operator VkCommandPool &() { return commandPool; }
};

} // namespace BR
