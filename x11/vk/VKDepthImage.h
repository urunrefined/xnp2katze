#pragma once

#include "VKFramebuffer.h"
#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKImageView.h"
#include "VKUtil.h"

namespace BR {
class VulkanDepthImage {
  public:
    uint16_t width;
    uint16_t height;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VulkanImage texture;
    VulkanImageMemory textureMemory;
    VulkanImageView textureView;

    VulkanDepthImage(VkDevice device, VkPhysicalDevice physicalDevice,
                     uint16_t width, uint16_t height, VkFormat format);
};

} // namespace BR
