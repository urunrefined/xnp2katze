#include "VKCommandBuffer.h"
#include "VKCommandPool.h"
#include "VKTexture.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace BR {

void drawAndWait(VkDevice device, VulkanCommandBuffer &renderBuffer,
                 std::vector<VulkanCmbBuffer *> &cmbBuffers,
                 VulkanCommandPool &pool,
                 std::vector<VulkanTextureGeneric *> &textures,
                 VkQueue graphicsQueue);

void renderStart(VkCommandBuffer commandBuffer, VkRenderPass renderPass,
                 VkExtent2D extent, VkFramebuffer framebuffer);

void renderEnd(VkCommandBuffer buffer);

void finalizeCommandBuffer(VkCommandBuffer buffer);

} // namespace BR
