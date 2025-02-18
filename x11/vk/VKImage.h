#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanImage {
    const VkDevice &device;

  public:
    VkImage image;

    VulkanImage(const VkDevice &device_, uint32_t width, uint32_t height,
                VkFormat format, VkImageUsageFlags usage);
    ~VulkanImage();

    operator VkImage &() { return image; }
};

} // namespace BR

