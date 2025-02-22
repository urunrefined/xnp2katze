#pragma once

#include "VKPipeline.h"
#include "VKRenderBuffer.h"
#include "VKRenderOptions.h"
#include "VKShaders.h"

namespace BR {

class PipelineTexExtIy {

  public:
    VulkanPipelineLayout pipelineLayout;
    VulkanPipeline pipeline;

    PipelineTexExtIy(VkDevice device, ShaderStore &shader3D,
                     const RenderOptions &renderOptions,
                     VkRenderPass renderPass,
                     VkDescriptorSetLayout desciptorSetLayout);

    ~PipelineTexExtIy();

    void record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                const VkDescriptorSet &descriptorSet, VkBuffer vertices,
                VkDeviceSize verticesOffset, VkBuffer uvs,
                VkDeviceSize uvOffset, size_t drawCount);
};

} // namespace BR
