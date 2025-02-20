#pragma once

#include "VKBuffers.h"
#include "VKCommandBuffer.h"
#include "VKCommandPool.h"
#include "VKDevice.h"
#include "VKInstance.h"
#include "VKPhysicalDevice.h"
#include "VKRenderBuffer.h"
#include "VKRenderPass.h"
#include "VKRenderer.h"
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
    VulkanSemaphore renderFinishedSemaphore;
    VulkanSemaphore vboUpdatedSemaphore;

    RenderSemaphores(VulkanDevice &device)
        : imageAvailableSemaphore(device), renderFinishedSemaphore(device),
          vboUpdatedSemaphore(device) {}
};

class VulkanScaler {
  public:
    VulkanDevice &device;

    VulkanCommandPool pool;
    std::unique_ptr<VulkanCommandBuffer> commandBuffer;

    Sitter imageSitter;
    Sitter queueSitter;

    RenderSemaphores renderSemaphores;

    VulkanScaler(VulkanDevice &device)
        : device(device), pool(device, device.graphicsFamily),
          commandBuffer(nullptr), imageSitter(device), queueSitter(device),
          renderSemaphores(device) {}

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
};

} // namespace BR
