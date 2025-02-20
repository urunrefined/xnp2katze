#include "VKSwapChainFramebuffers.h"

namespace BR {

VulkanSwapChainFramebuffers::VulkanSwapChainFramebuffers(
    VkDevice device, VkPhysicalDevice physicalDevice,
    VulkanSwapChain &swapChain, VkRenderPass renderPass, VkFormat depthFormat)

    : depthImage(device, physicalDevice, swapChain.extent.width,
                 swapChain.extent.height, depthFormat) {

    uint32_t imageCount;

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

    for (auto &image : images) {
        imageViews.push_back(std::make_unique<VulkanImageView>(
            device, image, swapChain.format, VK_IMAGE_ASPECT_COLOR_BIT));
    }

    for (auto &imageView : imageViews) {
        std::array<VkImageView, 2> attachments = {*imageView,
                                                  depthImage.textureView};

        framebuffers.push_back(std::make_unique<VulkanFramebuffer>(
            device, swapChain.extent, renderPass, attachments));
    }
}

} // namespace BR
