#include "VKImageView.h"
#include <vulkan/vulkan_core.h>

namespace BR {

VulkanImageView::VulkanImageView(VkDevice device, VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspectFlags)
    : device(device) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) !=
        VK_SUCCESS) {
        throw "failed to create texture image view!";
    }
}

VulkanImageView::~VulkanImageView() {
    vkDestroyImageView(device, imageView, nullptr);
}

} // namespace BR
