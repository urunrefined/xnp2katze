#include "vk/VKDescriptorPool.h"
#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"

#include "pulse/PulseSoundEngine.h"

#include "exception.h"
#include "inputhandling.h"
#include "loop.h"
#include "np2.h"
#include "pccore.h"
#include "pccorecallbacks.h"
#include "util/Matrix4x4.h"
#include "util/Vertex.h"

namespace BR {

static VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanContext &engine) {
    BR::VulkanPhysicalDeviceEnumerations physicalDeviceEnums(engine.instance);

    for (BR::VulkanPhysicalDevice &physicalDevice :
         physicalDeviceEnums.physicalDevices) {
        // Use the first one available

        if (physicalDevice.isDeviceSuitable(engine.surface)) {
            return physicalDevice;
        }
    }

    throw Exception("No graphics device suitable");
}

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

static Vec2 posstatic[6]{
    {-1.0, 1.0},  // lower left
    {-1.0, -1.0}, // upper left
    {1.0, -1.0},  // upper right

    {-1.0, 1.0}, // lower left
    {1.0, -1.0}, // upper right
    {1.0, 1.0}   // lower right
};

static void glLoop(SignalFD &sfd, InputMapper &inputMapper,
                   VulkanContext &engine, VulkanPhysicalDevice &physicalDevice,
                   Sfx::PulseSoundEngine &soundEngine, NP2CFG &cfg,
                   NP2OSCFG &oscfg) {
    VisualScreen visualScreen = VisualScreen::MAIN;
    DoubleLines doubleLines = DoubleLines::NO;

    VulkanScaler scaler(engine, physicalDevice);
    std::unique_ptr<VulkanRenderBuffer> renderBuffer;

    VulkanTexture mainTexture(
        scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
        scaler.renderer.graphicsFamily, pc98Width, pc98Height);

    VulkanDescriptorPool descriptorPool(scaler.device, 1);

    VulkanDescriptorSet descriptorSetMain(
        scaler.device, mainTexture.textureView, scaler.renderer.sampler,
        descriptorPool, scaler.renderer.descriptorLayout);

    CallbackContext ctx{&mainTexture, &scaler.context.glfwCtx.input};

    ViewPortMode mode = ViewPortMode::INTEGER;

    std::vector<VulkanCmbBuffer *> cmbBuffers;

    while (scaler.getWindowState() != WindowState::SHOULDCLOSE &&
           !sfd.isTriggered()) {
        mainloop(&ctx, &soundEngine);

        if (scaler.renderingComplete()) {

            mainTexture.update(doubleLines);

            renderBuffer = scaler.newRenderBuffer();
            scaler.pollWindowEvents();

            renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);

            if (visualScreen == VisualScreen::CFG) {
            
            } else {
                if (mode == ViewPortMode::ASPECT) {
                    scaler.renderer.pipelineAspect->record(
                        *renderBuffer, descriptorSetMain, 6);
                } else if (mode == ViewPortMode::STRETCH) {
                    scaler.renderer.pipelineStretch->record(
                        *renderBuffer, descriptorSetMain, 6);
                } else {
                    scaler.renderer.pipelineInteger->record(
                        *renderBuffer, descriptorSetMain, 6);
                }
            }

            renderBuffer->end();

            if (scaler.drawAndPresent(*renderBuffer, cmbBuffers) ==
                RenderState::NEEDSSWAPCHAINUPDATE) {
                scaler.recreateSwapChain();
            }
        }

        GLFWInput &input = engine.glfwCtx.getInput();

        inputMapper.handleInput(input, mode, visualScreen, doubleLines,
                                soundEngine);

        input.reset();
    }

    while (!scaler.renderingComplete()) {
        usleep(1000);
    }

    vkDeviceWaitIdle(scaler.device);
}

void loop(SignalFD &sfd, InputMapper &inputMapper, NP2CFG &cfg, NP2OSCFG &oscfg,
          Sfx::PulseSoundEngine &soundEngine) {
#ifndef VKDEBUG
    VulkanContext engine(false);
#else
    VulkanContext engine(true);
#endif
    VulkanPhysicalDevice physicalDevice = glPhysicalDeviceSelection(engine);

    glLoop(sfd, inputMapper, engine, physicalDevice,
           soundEngine, cfg, oscfg);
}

} // namespace BR
