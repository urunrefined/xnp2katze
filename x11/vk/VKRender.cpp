#include "VKRender.h"

#include "VKBuffers.h"
#include "VKSync.h"

#include "util/Range.h"
#include "vk/VKCommandBuffer.h"
#include "vk/VKCommandPool.h"
#include "vk/VKTexture.h"

#include <assert.h>

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include <stdio.h>
#include <vulkan/vulkan_core.h>

namespace BR {

static void copyBuffer(const std::vector<Range> &ranges,
                       VkCommandBuffer &commandBuffer,
                       VulkanCmbBuffer &gBuffer) {
    if (!ranges.empty()) {
        VkBufferCopy buffers[ranges.size()];

        for (size_t i = 0; i < ranges.size(); i++) {
            buffers[i] = {ranges[i].start, ranges[i].start, ranges[i].size};
        }

        vkCmdCopyBuffer(commandBuffer, gBuffer.stagingBuffer,
                        gBuffer.bufferCard, (uint32_t)ranges.size(), buffers);
    }
}

void renderStart(VkCommandBuffer commandBuffer, VkRenderPass renderPass,
                 VkExtent2D extent, VkFramebuffer framebuffer) {

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void renderEnd(VkCommandBuffer buffer) { vkCmdEndRenderPass(buffer); }

void finalizeCommandBuffer(VkCommandBuffer buffer) {
    if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        throw "failed to record command buffer!";
    }
}

void drawAndWait(VkDevice device, VulkanCommandBuffer &renderBuffer,
                 std::vector<VulkanCmbBuffer *> &cmbBuffers,
                 VulkanCommandPool &pool,
                 std::vector<VulkanTextureGeneric *> &textures,
                 VkQueue graphicsQueue) {

    VulkanSemaphore renderFinishedSemaphore(device);
    VulkanSemaphore vboUpdatedSemaphore(device);
    const VulkanSemaphore imageFinishedSemaphore(device);

    Sitter queueSitter(device);

    std::unique_ptr<VulkanCommandBuffer> commandBuffer =
        std::make_unique<VulkanCommandBuffer>(device, pool, graphicsQueue);

    for (VulkanCmbBuffer *cmbBuffer : cmbBuffers) {
        copyBuffer(cmbBuffer->ranges.ranges, *commandBuffer, *cmbBuffer);
    }

    for (VulkanTextureGeneric *texture : textures) {
        texture->update(*commandBuffer);
    }

    (*commandBuffer).end();

    VkSubmitInfo submitInfo[2] = {};
    const VkPipelineStageFlags flagsWaitForImage =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    {
        VkSubmitInfo &updateSubmitInfo = submitInfo[0];
        updateSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        updateSubmitInfo.commandBufferCount = 1;
        updateSubmitInfo.pCommandBuffers = &(commandBuffer->commandBuffer);

        updateSubmitInfo.waitSemaphoreCount = 0;
        updateSubmitInfo.pWaitSemaphores = 0;
        updateSubmitInfo.pWaitDstStageMask = &flagsWaitForImage;

        updateSubmitInfo.signalSemaphoreCount = 1;
        updateSubmitInfo.pSignalSemaphores = vboUpdatedSemaphore;
    }

    const VkPipelineStageFlags flagsWaitForVBOUpdate =
        VK_PIPELINE_STAGE_TRANSFER_BIT;

    {
        VkSubmitInfo &drawSubmitInfo = submitInfo[1];
        drawSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        drawSubmitInfo.waitSemaphoreCount = 1;
        drawSubmitInfo.pWaitSemaphores = vboUpdatedSemaphore;
        drawSubmitInfo.pWaitDstStageMask = &flagsWaitForVBOUpdate;

        drawSubmitInfo.commandBufferCount = 1;
        drawSubmitInfo.pCommandBuffers = &(renderBuffer.commandBuffer);

        drawSubmitInfo.signalSemaphoreCount = 1;
        drawSubmitInfo.pSignalSemaphores = renderFinishedSemaphore;
    }

    queueSitter.activate();

    if (vkQueueSubmit(graphicsQueue, 2, submitInfo, queueSitter) !=
        VK_SUCCESS) {
        throw "failed to submit draw command buffer!";
    }

    for (VulkanCmbBuffer *cmbBuffer : cmbBuffers) {
        cmbBuffer->ranges.ranges.clear();
    }

    for (VulkanTextureGeneric *texture : textures) {
        texture->clean();
    }

    queueSitter.block();
}

} // namespace BR
