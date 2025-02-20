#pragma once

#include "VKCommandPool.h"
#include "VKDevice.h"
#include "VKFramebuffer.h"
#include "VKRenderPass.h"
#include <memory>

namespace BR {

class VulkanRenderBuffer {
  public:
    VkDevice device;

    VulkanCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VulkanRenderBuffer(VkDevice device, uint32_t graphicsFamily)
        : device(device), commandPool(device, graphicsFamily) {}

    void begin(VulkanRenderPass &renderPass,
               std::vector<std::unique_ptr<VulkanFramebuffer>> &framebuffers,
               VkExtent2D extent);
    void end();
};

} // namespace BR
