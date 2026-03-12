#include "PicContext.h"
#include "np2.h"
#include "util/Deep.h"

namespace BR {

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

PicContext::PicContext(VulkanDevice &device,
                       VulkanPhysicalDevice &physicalDevice,
                       std::unique_ptr<VulkanWindowContext> &&windowContext_,
                       VulkanSampler &sampler, VulkanDescriptorLayouts &layouts,
                       Sfx::PulseSoundEngine &soundEngine,
                       InputMapper &inputMapper, Mutex &globalMutex)
    : MainContext(device, physicalDevice, std::move(windowContext_)),
      uniformBuffer(device, physicalDevice, 2 * 1024 * 1024), ua(uniformBuffer),
      mainTexture(device, physicalDevice, pc98Width, pc98Height),
      descriptorPool(device, 1),
      descriptorSetMain(device, mainTexture.textureView, sampler,
                        descriptorPool, layouts.descriptorLayout),
      img(pc98Width * pc98Height * 4, 255),
      ctx{{pc98Width, pc98Height, img.data()},
          &windowContext->glfwSurface.input,
          false},
      soundEngine(soundEngine), inputMapper(inputMapper),
      globalMutex(globalMutex)

{
    fill(mainTexture, {000, 000, 000, 0});
    fill(mainTexture, {20, 20, 40, 40}, {255, 0, 000, 255});
    fill(mainTexture, {0, 80, 1024, 40}, {0, 0, 255, 255});

    mainTexture.dirty();

    textures.push_back(&mainTexture);
}

void PicContext::createRender(VulkanRenderPass &renderPass,
                              VkFormat renderDepthFormat,
                              ShaderStore &shaderStore,
                              VulkanDescriptorLayouts &layouts) {
    MainContext::createRender(renderPass, renderDepthFormat);

    RenderOptions renderOptions{
        VK_FALSE, VK_FALSE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_POLYGON_MODE_FILL,
        getIntegerScissor(pc98Width, pc98Height,
                          renderContext->swapChain.extent.width,
                          renderContext->swapChain.extent.height)};

    pipeline =
        std::make_unique<PipelineTex>(device, shaderStore, renderOptions,
                                      renderPass, layouts.descriptorLayout);
}

void PicContext::work() {
    Input &input = getInput();

    if (input.getButton(KeyButtons::KEY_SUPER)) {
        inputMapper.handleSpecialInput(input, viewPortMode, visualScreen,
                                       doubleLines, soundEngine);
    } else {
        inputMapper.handleInputKeys(input);
    }

    LockGuard lock(globalMutex);
    mainloop(&ctx, &soundEngine);
}

void PicContext::frame() {
    if (ctx.dirty) {

        ctx.dirty = false;

        if (doubleLines == DoubleLines::SIMPLE) {
            doubleBlankLines(pc98Width, pc98Height,
                             (const unsigned char *)img.data(),
                             (unsigned char *)mainTexture.data);
        } else if (doubleLines == DoubleLines::ADAPTIVE) {
            doubleBlankLinesAdaptive(pc98Width, pc98Height,
                                     (const unsigned char *)img.data(),
                                     (unsigned char *)mainTexture.data);
        } else {
            memcpy(mainTexture.data, img.data(), img.size());
        }

        mainTexture.dirty();
        needsUpdate = true;
    }
}

void PicContext::record() {
    pipeline->record((*renderBuffer).commandBuffers.data(),
                     (*renderBuffer).commandBuffers.size(), descriptorSetMain,
                     6);
}

PicContext::~PicContext() {}

} // namespace BR
