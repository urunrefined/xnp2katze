#pragma once

#include "VKRenderBuffer.h"
#include "VKShaders.h"
#include "VKUtil.h"

namespace BR {

class VulkanPipelineLayout {
    VkDevice device;

  public:
    VkPipelineLayout pipelineLayout;

    VulkanPipelineLayout(VkDevice device, VkDescriptorSetLayout layout);
    ~VulkanPipelineLayout();

    operator VkPipelineLayout &() { return pipelineLayout; }
    operator VkPipelineLayout *() { return &pipelineLayout; }
};

class VulkanPipeline {
    VkDevice device;

  public:
    VkPipeline pipeline;

    VulkanPipeline(VkDevice device, ShaderStage &vert, ShaderStage &frag,
                   const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
                   const VkRect2D &scissor, VkRenderPass renderPass,
                   VkPipelineLayout pipelineLayout, VkBool32 depthEnable,
                   VkBool32 blendEnable, VkPolygonMode polygonMode,
                   VkPrimitiveTopology topology);

    ~VulkanPipeline();

    operator VkPipeline &() { return pipeline; }
    operator VkPipeline *() { return &pipeline; }
};

} // namespace BR
