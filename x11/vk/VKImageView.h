#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanImageView {
    VkImageView imageView;
    VkDevice device;

  public:
    VulkanImageView(const VulkanImageView &imageView) = delete;

    VulkanImageView(VkDevice device, VkImage image, VkFormat format,
                    VkImageAspectFlags aspectFlags);
    ~VulkanImageView();

    operator VkImageView &() { return imageView; }
};

} // namespace BR
