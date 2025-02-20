#pragma once

#include <vulkan/vulkan.h>

#include <array>

namespace BR {

class VulkanFramebuffer {
    VkFramebuffer framebuffer;
    VkDevice device;

  public:
    VulkanFramebuffer(const VulkanFramebuffer &imageView) = delete;
    VulkanFramebuffer(VkDevice device, const VkExtent2D &extent,
                      VkRenderPass renderPass,
                      std::array<VkImageView, 2> &attachments);

    VulkanFramebuffer(VkDevice device, const VkExtent2D &extent,
                      VkRenderPass renderPass, VkImageView attachment);

    ~VulkanFramebuffer();

    operator VkFramebuffer &() { return framebuffer; }

    operator const VkFramebuffer &() const { return framebuffer; }
};

} // namespace BR
