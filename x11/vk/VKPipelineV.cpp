#include "VKPipelineV.h"

namespace BR {

static VkPipelineVertexInputStateCreateInfo getVertexInfo() {
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    return vertexInputInfo;
}

PipelineV::PipelineV(VkDevice device, ShaderStore &shader3D,
                     const RenderOptions &renderOptions,
                     VkRenderPass renderPass)
    : pipelineLayout(device, 0),
      pipeline(device, shader3D.vertTri, shader3D.fragTri, getVertexInfo(),
               renderOptions.scissor, renderPass, pipelineLayout.pipelineLayout,
               renderOptions.depth, renderOptions.blend, renderOptions.polyMode,
               renderOptions.topology) {}

PipelineV::~PipelineV() {}

void PipelineV::record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                       size_t drawCount) {
    if (!drawCount)
        return;

    for (size_t i = 0; i < bufferCount; i++) {
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        vkCmdDraw(commandBuffers[i], (uint32_t)drawCount, 1, 0, 0);
    }
}

} // namespace BR
