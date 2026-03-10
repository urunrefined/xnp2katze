#include "VKDepthImage.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace BR {

VulkanDepthImage::VulkanDepthImage(VkDevice device,
                                   VkPhysicalDevice physicalDevice,
                                   uint16_t width, uint16_t height,
                                   VkFormat format)
    : width(width), height(height), device(device),
      physicalDevice(physicalDevice),
      texture(device, width, height, format,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
      textureMemory(physicalDevice, device, texture,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      textureView(device, texture, format, VK_IMAGE_ASPECT_DEPTH_BIT) {}
} // namespace BR
