#pragma once

#include "VKDepthImage.h"
#include "VKSwapChain.h"

#include <memory>
#include <vector>

namespace BR {

class VulkanSwapChainFramebuffers {
  public:
    VulkanDepthImage depthImage;
    std::vector<VkImage> images;
    std::vector<std::unique_ptr<VulkanImageView>> imageViews;
    std::vector<std::unique_ptr<VulkanFramebuffer>> framebuffers;

    VulkanSwapChainFramebuffers(VkDevice device,
                                VkPhysicalDevice physicalDevice,
                                VulkanSwapChain &swapChain,
                                VkRenderPass renderPass, VkFormat depthFormat);
};

} // namespace BR
