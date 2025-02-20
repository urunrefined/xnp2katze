#pragma once

#include "VKPipeline.h"
#include "VKRenderBuffer.h"
#include "VKShaders.h"

namespace BR {

class PipelineV {

  public:
    VulkanPipelineLayout pipelineLayout;
    VulkanPipeline pipeline;

    PipelineV(VkDevice device, ShaderStore &shader3D, const VkRect2D &scissor,
              VkRenderPass renderPass);

    ~PipelineV();

    void record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                size_t drawCount);
};

} // namespace BR
