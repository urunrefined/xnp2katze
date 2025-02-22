#include "VKPipelineTex.h"

namespace BR {

static VkPipelineVertexInputStateCreateInfo getVertexInfo() {
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    return vertexInputInfo;
}

PipelineTex::PipelineTex(VkDevice device, ShaderStore &shader3D,
                         const RenderOptions &renderOptions,
                         VkRenderPass renderPass,
                         VkDescriptorSetLayout desciptorSetLayout)
    : pipelineLayout(device, desciptorSetLayout),
      pipeline(device, shader3D.vertTex, shader3D.fragTex, getVertexInfo(),
               renderOptions.scissor, renderPass, pipelineLayout.pipelineLayout,
               renderOptions.depth, renderOptions.blend, renderOptions.polyMode,
               renderOptions.topology) {}

PipelineTex::~PipelineTex() {}

void PipelineTex::record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                         VkDescriptorSet descriptorSet, size_t drawCount) {
    if (!drawCount)
        return;

    for (size_t i = 0; i < bufferCount; i++) {

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        vkCmdBindDescriptorSets(commandBuffers[i],
                                VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                0, 1, &descriptorSet, 0, nullptr);

        vkCmdDraw(commandBuffers[i], (uint32_t)drawCount, 1, 0, 0);
    }
}

} // namespace BR
