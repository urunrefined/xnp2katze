#include "VKFramebuffer.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace BR {

VulkanFramebuffer::VulkanFramebuffer(VkDevice device, const VkExtent2D &extent,
                                     VkRenderPass renderPass,
                                     std::array<VkImageView, 2> &attachments)
    : device(device) {
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = (uint32_t)attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        throw "failed to create framebuffer!";
    }
}

VulkanFramebuffer::VulkanFramebuffer(VkDevice device, const VkExtent2D &extent,
                                     VkRenderPass renderPass,
                                     VkImageView attachment)
    : device(device) {
    VkFramebufferCreateInfo framebufferInfo;
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.pNext = NULL;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &attachment;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;
    framebufferInfo.flags = 0;

    vkCreateFramebuffer(device, &framebufferInfo, NULL, &framebuffer);
}

VulkanFramebuffer::~VulkanFramebuffer() {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
}

} // namespace BR
