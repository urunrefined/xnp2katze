#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanCommandBuffer {
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;

  public:
    VkCommandBuffer commandBuffer;

    VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool,
                        VkQueue graphicsQueue);

    void end();
    void submit();
    ~VulkanCommandBuffer();

    operator VkCommandBuffer &() { return commandBuffer; }
};

} // namespace BR
