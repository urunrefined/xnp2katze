#include "VKCommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace BR {

VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device,
                                         VkCommandPool commandPool,
                                         VkQueue graphicsQueue)
    : device(device), commandPool(commandPool), graphicsQueue(graphicsQueue) {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void VulkanCommandBuffer::submit() {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
}

void VulkanCommandBuffer::end() { vkEndCommandBuffer(commandBuffer); }

VulkanCommandBuffer::~VulkanCommandBuffer() {
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

} // namespace BR
