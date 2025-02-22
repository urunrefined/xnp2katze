#include "vk/VKDebug.h"
#include "vk/VKDescriptorLayouts.h"
#include "vk/VKDescriptorPool.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/VKDevice.h"
#include "vk/VKInstance.h"
#include "vk/VKPhysicalDeviceEnumerations.h"
#include "vk/VKPipelineTex.h"
#include "vk/VKSampler.h"
#include "vk/VKScaler.h"
#include "vk/VKShaders.h"
#include "vk/VKSwapChain.h"
#include "vk/VKSwapChainFramebuffers.h"
#include "vk/VKUtil.h"

#include "glfw/VKGLFW.h"
#include "glfw/VKSurface.h"

#include "gl/GLConsole.h"
#include "gl/GLGlyphCache.h"

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

#ifdef VKDEBUG
static int vkdebug = 1;
#else
static int vkdebug = 0;
#endif

static const char *validationLayerString = "VK_LAYER_KHRONOS_validation";

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

/*

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
}*/

void loop(SignalFD &sfd, InputMapper &inputMapper, NP2CFG &cfg, NP2OSCFG &oscfg,
          Sfx::PulseSoundEngine &soundEngine) {

    vkdebug = 1;

    GLFWContext glfwContext(640, 400);

    auto extensions = getRequiredGLFWExtensions();
    std::vector<const char *> layers;

    if (vkdebug) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        layers.push_back(validationLayerString);
    }

    VulkanInstance instance(extensions, layers);
    VulkanDebugCallback callback(instance, vkdebug);
    VulkanSurface surface(glfwContext, instance);

    VulkanPhysicalDevice physicalDevice =
        glPhysicalDeviceSelection(instance, surface);

    VulkanDevice device(vkdebug, physicalDevice, surface,
                        physicalDevice.queueFamilies);

    ShaderStore shaderStore(device);

    VulkanDescriptorLayouts layouts(device);

    VulkanDescriptorPoolExt descriptorPoolExt(device, 32);
    VulkanSampler sampler(physicalDevice, device, VK_FILTER_NEAREST);
    size_t usSize = VulkanDescriptorSetExt::getChunkSize(physicalDevice, 32);

    VkFormat renderDepthFormat = findDepthFormat(physicalDevice);

    // TODO: UniformBuffer needs to be newly calculated
    VulkanUniformBuffer uniformBuffer(device, physicalDevice, 2 * 1024 * 1024);
    UniformAllocator ua(uniformBuffer);

    GlyphCache glyphCache(physicalDevice, device);

    VulkanVtxBuffer vtx(device, physicalDevice, 1024 * 1024 * 20);
    DataAllocator alc(vtx);

    std::string fontfile = getFont();
    printf("using font %s\n", fontfile.c_str());
    Font font(fontfile.c_str(), 128);

    FontContext fontContext{glyphCache.textGlyphMappingCache,
                            glyphCache.imageIndexed, font.freetypeFace,
                            font.hbfont};

    GLConsole console(alc, fontContext, 80, device, physicalDevice, ua, sampler,
                      layouts, glyphCache.alphaTexture.textureView);

    std::vector<VulkanCmbBuffer *> cmbBuffers;
    cmbBuffers.push_back(&vtx);
    cmbBuffers.push_back(&uniformBuffer);

    std::vector<VulkanTextureGeneric *> textures;
    textures.push_back(&glyphCache.alphaTexture);

    auto dims = glfwContext.getCurrentSize();

    std::vector<uint32_t> codePoints;

    std::unique_ptr<VulkanSwapChain> swapChain(
        std::make_unique<VulkanSwapChain>(
            device, physicalDevice, device.graphicsFamily, device.presentFamily,
            surface, glfwContext.currentWidth, glfwContext.currentHeight,
            nullptr));

    std::unique_ptr<VulkanRenderPass> renderPass(
        std::make_unique<VulkanRenderPass>(
            device, swapChain->format, renderDepthFormat, ShouldPresent::YES));

    std::unique_ptr<VulkanSwapChainFramebuffers> swapChainFramebuffers(

        std::make_unique<VulkanSwapChainFramebuffers>(device, physicalDevice,
                                                      *swapChain, *renderPass,
                                                      renderDepthFormat));

    RenderOptions renderOptions = {

        VK_TRUE, VK_TRUE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_POLYGON_MODE_FILL,
        getIntegerScissor(pc98Width, pc98Height, swapChain->extent.width,
                          swapChain->extent.height)};

    std::unique_ptr<PipelineTex> pipeline =
        std::make_unique<PipelineTex>(device, shaderStore, renderOptions,
                                      *renderPass, layouts.descriptorLayout);

    std::unique_ptr<VulkanScaler> scaler =
        std::make_unique<VulkanScaler>(device);

    std::unique_ptr<VulkanRenderBuffer> renderBuffer;

    console.ready();

    bool needsUpdate = true;
    bool needsSwapchainUpdate = false;

    std::vector<char> img(pc98Width * pc98Height * 4, 255);

    VulkanDescriptorPool descriptorPool(scaler->device, 1);

    VulkanTextureBGRA mainTexture(device, physicalDevice, pc98Width,
                                  pc98Height);

    textures.push_back(&mainTexture);

    VulkanDescriptorSet descriptorSetMain(
        scaler->device, mainTexture.textureView, sampler, descriptorPool,
        layouts.descriptorLayout);

    CallbackContext ctx{
        {pc98Width, pc98Height, img.data()}, &glfwContext.input, false};

    while (glfwContext.getWindowState() != WindowState::SHOULDCLOSE &&
           !sfd.isTriggered()) {

        mainloop(&ctx, &soundEngine);

        if (!needsSwapchainUpdate && !needsUpdate) {
            // glfwContext.wait(1);
        }

        auto nDims = glfwContext.getCurrentSize();

        glfwContext.pollWindowEvents();

        if (nDims != dims) {
            needsSwapchainUpdate = true;
            dims = nDims;
        }

        GLFWInput &input = glfwContext.input;

        if (!input.codepoints.empty()) {
            console.add(input.codepoints);
            needsUpdate = true;
        }

        input.reset();

        if (scaler->renderingComplete()) {
            bool hasData = false;

            if (ctx.dirty) {

                ctx.dirty = false;

                memcpy(mainTexture.data, img.data(), img.size());

                for (size_t i = 3; i < img.size(); i += 4) {
                    ((char *)mainTexture.data)[i] = 255;
                }

                mainTexture.dirty();
                needsUpdate = true;
            }

            if (needsSwapchainUpdate) {
                if (glfwContext.currentWidth == 0 ||
                    glfwContext.currentHeight == 0) {
                    continue;
                }

                swapChain = std::make_unique<VulkanSwapChain>(
                    device, physicalDevice, device.graphicsFamily,
                    device.presentFamily, surface, glfwContext.currentWidth,
                    glfwContext.currentHeight, swapChain.get());

                renderPass = std::make_unique<VulkanRenderPass>(
                    device, swapChain->format, renderDepthFormat,
                    ShouldPresent::YES);

                swapChainFramebuffers =
                    std::make_unique<VulkanSwapChainFramebuffers>(
                        device, physicalDevice, *swapChain, *renderPass,
                        renderDepthFormat);

                /*

                         */

                renderOptions.scissor = getIntegerScissor(pc98Width, pc98Height,
                                  swapChain->extent.width,
                                  swapChain->extent.height);
                pipeline = std::make_unique<PipelineTex>(
                    device, shaderStore, renderOptions, *renderPass,
                    layouts.descriptorLayout);

                scaler = std::make_unique<VulkanScaler>(device);

                needsSwapchainUpdate = false;
                needsUpdate = true;
            }

            if (needsUpdate) {
                renderBuffer = std::make_unique<VulkanRenderBuffer>(
                    device, device.graphicsFamily);

                renderBuffer->begin(*renderPass,
                                    swapChainFramebuffers->framebuffers,
                                    swapChain->extent);

                pipeline->record(renderBuffer->commandBuffers.data(),
                                 renderBuffer->commandBuffers.size(),
                                 descriptorSetMain, 6);

                /*
                console.draw(*renderer, renderBuffer->commandBuffers.data(),
                             renderBuffer->commandBuffers.size());
                */

                renderBuffer->end();

                auto drawRet = scaler->draw(*renderBuffer, cmbBuffers, textures,
                                            *swapChain);

                if (drawRet.state == RenderState::NEEDSSWAPCHAINUPDATE) {
                    needsSwapchainUpdate = true;
                } else if (drawRet.state == RenderState::OK) {
                    scaler->present(drawRet.index, *swapChain);
                    needsUpdate = false;
                }
            }
        }
    }

    while (!scaler->renderingComplete()) {
        usleep(1000);
    }

    vkDeviceWaitIdle(device);
}

} // namespace BR
