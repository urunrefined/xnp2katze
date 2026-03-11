#pragma once

#include "vk/VKContext.h"
#include "vk/VKDescriptorLayouts.h"
#include "vk/VKDescriptorPool.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/VKDevice.h"
#include "vk/VKInstance.h"
#include "vk/VKPipelineTex.h"
#include "vk/VKSampler.h"
#include "vk/VKSwapChain.h"
#include "vk/VKSwapChainFramebuffers.h"
#include "vk/VKUtil.h"

#include "inputhandling.h"
#include "pccorecallbacks.h"
#include "pulse/PulseSoundEngine.h"
#include "util/Thread.h"

namespace BR {

class PicContext : public MainContext {
    // TODO: UniformBuffer needs to be newly calculated
    VulkanUniformBuffer uniformBuffer;
    UniformAllocator ua;
    VulkanTextureBGRA mainTexture;
    VulkanDescriptorPool descriptorPool;
    VulkanDescriptorSet descriptorSetMain;
    std::unique_ptr<PipelineTex> pipeline;
    std::vector<uint8_t> img;
    CallbackContext ctx;
    Sfx::PulseSoundEngine &soundEngine;
    InputMapper &inputMapper;

    VisualScreen visualScreen = VisualScreen::MAIN;
    DoubleLines doubleLines = DoubleLines::NO;
    ViewPortMode viewPortMode = ViewPortMode::INTEGER;

    Mutex &globalMutex;

  public:
    PicContext(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
               std::unique_ptr<VulkanWindowContext> &&windowContext_,
               VulkanSampler &sampler, VulkanDescriptorLayouts &layouts,
               Sfx::PulseSoundEngine &soundEngine, InputMapper &inputMapper,
               Mutex &globalMutex);

    virtual void createRender(VulkanRenderPass &renderPass,
                              VkFormat renderDepthFormat,
                              ShaderStore &shaderStore,
                              VulkanDescriptorLayouts &layouts) override;

    virtual void work() override;
    virtual void frame() override;
    virtual void record() override;
    virtual ~PicContext() override;
};

} // namespace BR
