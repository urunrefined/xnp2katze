#pragma once

#include "VKPipeline.h"
#include "VKRenderBuffer.h"
#include "VKRenderOptions.h"
#include "VKShaders.h"

namespace BR {

class PipelineTex {

  public:
    VulkanPipelineLayout pipelineLayout;
    VulkanPipeline pipeline;

    PipelineTex(VkDevice device, ShaderStore &shader3D,
                const RenderOptions &renderOptions, VkRenderPass renderPass,
                VkDescriptorSetLayout desciptorSetLayout);

    ~PipelineTex();

    void record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                VkDescriptorSet descriptorSet, size_t drawCount);
};

} // namespace BR
