#pragma once

#include "VKBuffers.h"
#include "VKCommandBuffer.h"
#include "VKCommandPool.h"
#include "VKDevice.h"
#include "VKInstance.h"
#include "VKPhysicalDevice.h"
#include "VKRenderBuffer.h"
#include "VKRenderPass.h"
#include "VKShaders.h"
#include "VKSync.h"
#include "VKTexture.h"

#include <memory>

namespace BR {

enum class RenderState {
    // OK :)
    OK,

    NEEDSSWAPCHAINUPDATE,
    // Can't do anything right now. Don't do anything.
    WAITING
};

struct RenderIdx {
    RenderState state;
    uint32_t index;
};

class RenderSemaphores {
    RenderSemaphores(RenderSemaphores &) = delete;

  public:
    VulkanSemaphore imageAvailableSemaphore;
    VulkanSemaphore vboUpdatedSemaphore;
    std::vector<VulkanSemaphore> renderFinishedSemaphores;

    RenderSemaphores(VulkanDevice &device, size_t count)
        : imageAvailableSemaphore(device), vboUpdatedSemaphore(device) {

        renderFinishedSemaphores.reserve(count);

        for (size_t i = 0; i < count; i++) {
            renderFinishedSemaphores.emplace_back(device);
        }
    }
};

class VulkanScaler {
  public:
    VulkanDevice &device;

    VulkanCommandPool pool;
    std::unique_ptr<VulkanCommandBuffer> commandBuffer;

    Sitter imageSitter;
    Sitter queueSitter;

    RenderSemaphores renderSemaphores;

    VulkanScaler(VulkanDevice &device, size_t imageCount)
        : device(device), pool(device, device.graphicsFamily),
          commandBuffer(nullptr), imageSitter(device), queueSitter(device),
          renderSemaphores(device, imageCount) {}

    bool renderingComplete() {
        if (!queueSitter.done()) {
            queueSitter.wait();
        }

        return queueSitter.done();
    }

    RenderIdx draw(VulkanRenderBuffer &renderBuffer,
                   std::vector<VulkanCmbBuffer *> &cmbBuffers,
                   std::vector<VulkanTextureGeneric *> &textures,
                   VkSwapchainKHR swapchainImages);

    void present(uint32_t idx, VkSwapchainKHR swapchainImages);

    RenderState drawAndPresent(VulkanRenderBuffer &renderBuffer,
                               std::vector<VulkanCmbBuffer *> &cmbBuffers,
                               std::vector<VulkanTextureGeneric *> &textures,
                               VkSwapchainKHR swapchainImages

    );

    ~VulkanScaler() { vkDeviceWaitIdle(device); }
};

} // namespace BR
