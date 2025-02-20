#pragma once

#include "VKPipeline.h"
#include "VKRenderBuffer.h"
#include "VKShaders.h"

namespace BR {

class PipelineTexExt {

  public:
    VulkanPipelineLayout pipelineLayout;
    VulkanPipeline pipeline;

    PipelineTexExt(const VkDevice device, ShaderStore &shader3D,
                   const VkRect2D &scissor, const VkRenderPass renderPass,
                   const VkDescriptorSetLayout desciptorSetLayout);

    ~PipelineTexExt();

    void record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                const VkDescriptorSet descriptorSet, const VkBuffer vertices,
                const VkDeviceSize verticesOffset, const VkBuffer uvs,
                VkDeviceSize uvOffset, size_t drawCount);
};

} // namespace BR
