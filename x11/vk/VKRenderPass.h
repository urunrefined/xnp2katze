#pragma once

#include <vulkan/vulkan.h>

namespace BR {

enum class ShouldPresent { YES, NO };

class VulkanRenderPass {
  public:
    VkDevice device;
    VkRenderPass renderPass;

    VulkanRenderPass(VkDevice device, VkFormat colorFormat,
                     VkFormat depthFormat, ShouldPresent present);
    ~VulkanRenderPass();

    operator VkRenderPass &() { return renderPass; }
};

} // namespace BR
