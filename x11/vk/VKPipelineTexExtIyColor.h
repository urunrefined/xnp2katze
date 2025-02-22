#pragma once

#include "VKPipeline.h"
#include "VKRenderBuffer.h"
#include "VKRenderOptions.h"
#include "VKShaders.h"

namespace BR {

class PipelineTexExtIyColor {

  public:
    VulkanPipelineLayout pipelineLayout;
    VulkanPipeline pipeline;

    PipelineTexExtIyColor(VkDevice device, ShaderStore &shader3D,
                          const RenderOptions &renderOptions,
                          VkRenderPass renderPass,
                          VkDescriptorSetLayout desciptorSetLayout);

    ~PipelineTexExtIyColor();

    void record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                VkDescriptorSet descriptorSet, VkBuffer vertices,
                VkDeviceSize verticesOffset, VkBuffer uvs,
                VkDeviceSize uvOffset, VkBuffer colors,
                VkDeviceSize colorOffset, size_t drawCount);
};

} // namespace BR
