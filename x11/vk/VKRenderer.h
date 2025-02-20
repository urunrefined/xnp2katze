#pragma once

#include "VKDescriptorLayouts.h"
#include "VKDescriptorSet.h"
#include "VKDescriptorSetExt.h"
#include "VKPhysicalDevice.h"
#include "VKPipelineTex.h"
#include "VKPipelineTexExt.h"
#include "VKPipelineTexExtIy.h"
#include "VKPipelineTexExtIyColor.h"
#include "VKPipelineV.h"
#include "VKRenderPass.h"
#include "VKSampler.h"
#include "VKShaders.h"
#include "VKUtil.h"

namespace BR {

class VulkanRenderer {
  public:
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    ShaderStore &shaderStore;

    std::unique_ptr<PipelineV> pipelineV;
    std::unique_ptr<PipelineTex> pipelineAspect;
    std::unique_ptr<PipelineTex> pipelineAspect1to1;
    std::unique_ptr<PipelineTex> pipelineStretch;
    std::unique_ptr<PipelineTex> pipelineInteger;
    std::unique_ptr<PipelineTexExt> pipelineExt1to1;
    std::unique_ptr<PipelineTexExt> pipelineExt16to9;
    std::unique_ptr<PipelineTexExt> pipelineStretchExt;
    std::unique_ptr<PipelineTexExtIy> pipelineExtIy16to9;
    std::unique_ptr<PipelineTexExtIyColor> pipelineExtIyColor16to9;

    VulkanDescriptorLayouts &layouts;

    VulkanRenderPass &renderPass;

    VulkanRenderer(VulkanPhysicalDevice &physicalDevice, VkDevice device,
                   ShaderStore &shaderStore, const VkExtent2D &swapChainExtent,
                   VulkanRenderPass &renderpass,
                   VulkanDescriptorLayouts &layouts);

    void reCreatePipeline(VkExtent2D swapChainExtent) {
        static const unsigned int pc98Width = 640;
        static const unsigned int pc98Height = 400;

        {
            VkRect2D scissor =
                getAspectScissor((double)pc98Width / (double)pc98Height,
                                 swapChainExtent.width, swapChainExtent.height);

            pipelineV = std::make_unique<PipelineV>(device, shaderStore,
                                                    scissor, renderPass);
            pipelineAspect = std::make_unique<PipelineTex>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayout);
        }

        {
            VkRect2D scissor = getAspectScissor(1, swapChainExtent.width,
                                                swapChainExtent.height);

            pipelineAspect1to1 = std::make_unique<PipelineTex>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayout);
        }

        {
            VkRect2D scissor = getAspectScissor(
                16.f / 9.f, swapChainExtent.width, swapChainExtent.height);

            pipelineExt16to9 = std::make_unique<PipelineTexExt>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayoutExt);

            pipelineExtIy16to9 = std::make_unique<PipelineTexExtIy>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayoutExt);

            pipelineExtIyColor16to9 = std::make_unique<PipelineTexExtIyColor>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayoutExt);
        }

        {
            VkRect2D scissor =
                get1to1Scissor(swapChainExtent.width, swapChainExtent.height);

            pipelineExt1to1 = std::make_unique<PipelineTexExt>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayoutExt);
        }

        {
            VkRect2D scissor =
                getIntegerScissor(pc98Width, pc98Height, swapChainExtent.width,
                                  swapChainExtent.height);

            pipelineInteger = std::make_unique<PipelineTex>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayout);
        }
        {

            VkRect2D scissor{{0, 0},
                             {swapChainExtent.width, swapChainExtent.height}};

            pipelineStretch = std::make_unique<PipelineTex>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayout);

            pipelineStretchExt = std::make_unique<PipelineTexExt>(
                device, shaderStore, scissor, renderPass,
                layouts.descriptorLayoutExt);
        }
    }

    virtual ~VulkanRenderer() {}
};

} // namespace BR
