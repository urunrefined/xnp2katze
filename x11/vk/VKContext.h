
#pragma once

#include "vk/VKDebug.h"
#include "vk/VKDescriptorLayouts.h"
#include "vk/VKDescriptorPool.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/VKDescriptorSetExt.h"
#include "vk/VKPhysicalDeviceEnumerations.h"
#include "vk/VKPipelineTex.h"
#include "vk/VKScaler.h"
#include "vk/VKSwapChain.h"
#include "vk/VKSwapChainFramebuffers.h"
#include "vk/VKTexture.h"

#include "glfw/VKGLFW.h"
#include "glfw/VKSurface.h"
#include "vk/VKUtil.h"

namespace BR {

class VulkanWindowContext {
  public:
    GLFWSurface glfwSurface;
    VulkanSurface vulkanSurface;

    VulkanWindowContext(VulkanInstance &instance, uint32_t sizeX,
                        uint32_t sizeY, const char *windowName)
        : glfwSurface(sizeX, sizeY, windowName), vulkanSurface(glfwSurface, instance) {}
};

class VulkanRenderContext {
  public:
    VulkanSwapChain swapChain;
    VulkanSwapChainFramebuffers swapChainFramebuffers;
    VulkanScaler scaler;

    VulkanRenderContext(VulkanDevice &device,
                        VulkanPhysicalDevice &physicalDevice,
                        GLFWSurface &glfwSurface, VulkanSurface &vulkanSurface,
                        VulkanRenderPass &renderPass,
                        VkFormat renderDepthFormat,
                        VulkanSwapChain *swapChainOld)
        :

          swapChain(device, physicalDevice, device.graphicsFamily,
                    device.presentFamily, vulkanSurface,
                    glfwSurface.currentWidth, glfwSurface.currentHeight,
                    swapChainOld),

          swapChainFramebuffers(device, physicalDevice, swapChain, renderPass,
                                renderDepthFormat),

          scaler(device, swapChainFramebuffers.images.size()) {}
};

class MainContext {
  public:
    VulkanDevice &device;
    VulkanPhysicalDevice &physicalDevice;

    std::unique_ptr<VulkanWindowContext> windowContext;
    std::unique_ptr<VulkanRenderContext> renderContext;

    Dimensions2D dims;
    std::vector<uint32_t> codePoints;

    bool needsSwapchainUpdate = false;
    bool needsUpdate = false;

    std::unique_ptr<VulkanRenderBuffer> renderBuffer;

    std::vector<VulkanTextureGeneric *> textures;
    std::vector<VulkanCmbBuffer *> cmbBuffers;

    MainContext(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
                std::unique_ptr<VulkanWindowContext> &&windowContext)
        : device(device), physicalDevice(physicalDevice),
          windowContext(std::move(windowContext)), dims({0, 0}) {}

    void createRender(VulkanRenderPass &renderPass,
                      VkFormat renderDepthFormat) {
        renderContext = std::make_unique<VulkanRenderContext>(
            device, physicalDevice, windowContext->glfwSurface,
            windowContext->vulkanSurface, renderPass, renderDepthFormat,
            nullptr);

        needsSwapchainUpdate = false;
        needsUpdate = true;
    }

    virtual void createRender(VulkanRenderPass &renderPass,
                              VkFormat renderDepthFormat,
                              ShaderStore &shaderStore,
                              VulkanDescriptorLayouts &layouts) = 0;

    bool isOutdated() {
        auto nDims = windowContext->glfwSurface.getCurrentSize();

        if (nDims != dims) {
            needsSwapchainUpdate = true;
            dims = nDims;
        }

        return needsSwapchainUpdate;
    }

    virtual void frame() = 0;

    Input &getInput() { return windowContext->glfwSurface.input; }

    void reset() {
        GLFWInput &input = windowContext->glfwSurface.input;
        input.reset();
    }

    void render(VulkanDevice &device, VulkanRenderPass &renderPass) {
        if (needsSwapchainUpdate)
            return;

        if (needsUpdate) {
            renderBuffer = std::make_unique<VulkanRenderBuffer>(
                device, device.graphicsFamily);

            renderBuffer->begin(
                renderPass, renderContext->swapChainFramebuffers.framebuffers,
                renderContext->swapChain.extent);

            record();

            renderBuffer->end();

            auto drawRet = renderContext->scaler.draw(
                *renderBuffer, cmbBuffers, textures, renderContext->swapChain);

            if (drawRet.state == RenderState::NEEDSSWAPCHAINUPDATE) {
                needsSwapchainUpdate = true;
            } else if (drawRet.state == RenderState::OK) {
                renderContext->scaler.present(drawRet.index,
                                              renderContext->swapChain);
                needsUpdate = false;
            }
        }
    }

    virtual void record() = 0;
    virtual void work() = 0;

    bool canCurrentlyRender() {
        return renderContext && renderContext->scaler.renderingComplete();
    }

    virtual ~MainContext() {}
};

} // namespace BR
