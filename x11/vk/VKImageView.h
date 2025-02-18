#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanImageView {
    VkImageView imageView;
    const VkDevice &device;

  public:
    VulkanImageView(const VulkanImageView &imageView) = delete;

    VulkanImageView(const VkDevice &device_, VkImage image, VkFormat format,
                    VkImageAspectFlags aspectFlags);
    ~VulkanImageView();

    operator VkImageView &() { return imageView; }
};
} // namespace BR
