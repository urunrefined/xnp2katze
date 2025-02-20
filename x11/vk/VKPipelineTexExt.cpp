#include "VKPipelineTexExt.h"
#include "util/Core.h"
#include "util/Vertex.h"

namespace BR {

static VkVertexInputBindingDescription bindingDescriptions[2] = {
    {.binding = 0,
     .stride = sizeof(Vec2),
     .inputRate = VK_VERTEX_INPUT_RATE_VERTEX},
    {
        .binding = 1,
        .stride = sizeof(Vec2),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    }};

static VkVertexInputAttributeDescription attributeDescriptions[2] = {
    {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = 0,
    },
    {
        .location = 1,
        .binding = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = 0,
    }};

static VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = 0,
    .flags = 0,
    .vertexBindingDescriptionCount = 2,
    .pVertexBindingDescriptions = bindingDescriptions,
    .vertexAttributeDescriptionCount = 2,
    .pVertexAttributeDescriptions = attributeDescriptions,
};

PipelineTexExt::PipelineTexExt(const VkDevice device, ShaderStore &shader3D,
                               const VkRect2D &scissor,
                               const VkRenderPass renderPass,
                               const VkDescriptorSetLayout desciptorSetLayout)
    : pipelineLayout(device, desciptorSetLayout),
      pipeline(device, shader3D.vertTexExt, shader3D.fragTex, vertexInputInfo,
               scissor, renderPass, pipelineLayout, VK_TRUE, VK_TRUE,
               VK_POLYGON_MODE_FILL, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)

{}

PipelineTexExt::~PipelineTexExt() {}

void PipelineTexExt::record(VkCommandBuffer *commandBuffers, size_t bufferCount,
                            const VkDescriptorSet descriptorSet,
                            const VkBuffer vertices,
                            VkDeviceSize verticesOffset, const VkBuffer uvs,
                            VkDeviceSize uvOffset, size_t drawCount) {
    if (!drawCount)
        return;

    for (size_t i = 0; i < bufferCount; i++) {

        VkBuffer buffers[2] = {vertices, uvs};
        VkDeviceSize bufferSizes[2] = {verticesOffset, uvOffset};

        vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, buffers, bufferSizes);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        vkCmdBindDescriptorSets(commandBuffers[i],
                                VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                0, 1, &descriptorSet, 0, nullptr);

        vkCmdDraw(commandBuffers[i], (uint32_t)drawCount, 1, 0, 0);
    }
}

} // namespace BR
