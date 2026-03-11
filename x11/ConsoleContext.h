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

#include "font/FontContext.h"
#include "gl/GLConsole.h"
#include "gl/GLGlyphCache.h"
#include "np2.h"
#include "pccorecallbacks.h"
#include "pulse/PulseSoundEngine.h"

namespace BR {

class ConsoleContext : public MainContext {
    // TODO: UniformBuffer needs to be newly calculated

    VulkanUniformBuffer uniformBuffer;
    UniformAllocator ua;
    GlyphCache glyphCache;

    VulkanVtxBuffer vtx;
    DataAllocator alc;

    std::unique_ptr<PipelineTexExtIyColor> pipeline;

    Font font;
    FontContext fontContext;

    GLConsole console;

    std::vector<uint32_t> codePoints;

    const std::string &diskDir;
    const NP2OSCFG &oscfg;

    Mutex &globalMutex;

  public:
    ConsoleContext(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
                   std::unique_ptr<VulkanWindowContext> &&windowContext_,
                   VulkanSampler &sampler, VulkanDescriptorLayouts &layouts,
                   const std::string &diskDir, const NP2OSCFG &oscfg,
                   Mutex &globalMutex);

    virtual void createRender(VulkanRenderPass &renderPass,
                              VkFormat renderDepthFormat,
                              ShaderStore &shaderStore,
                              VulkanDescriptorLayouts &layouts) override;

    virtual void work() override;
    virtual void frame() override;
    virtual void record() override;
    virtual ~ConsoleContext() override;
};

} // namespace BR
