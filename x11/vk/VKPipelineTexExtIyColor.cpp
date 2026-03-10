#include "VKPipelineTexExtIyColor.h"
#include "util/Core.h"
#include "util/Vertex.h"
#include "vk/VKRenderOptions.h"
#include "vk/VKShaders.h"
#include <cstddef>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace BR {

static VkVertexInputBindingDescription bindingDescriptions[] = {
    {.binding = 0,
     .stride = sizeof(Vec2),
     .inputRate = VK_VERTEX_INPUT_RATE_VERTEX},
    {
        .binding = 1,
        .stride = sizeof(Vec2),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    },
    {
        .binding = 2,
        .stride = sizeof(Vec3),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    }};

static VkVertexInputAttributeDescription attributeDescriptions[] = {
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
    },
    {
        .location = 2,
        .binding = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0,
    }};

static VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = 0,
    .flags = 0,
    .vertexBindingDescriptionCount = arraySize(bindingDescriptions),
    .pVertexBindingDescriptions = bindingDescriptions,
    .vertexAttributeDescriptionCount = arraySize(attributeDescriptions),
    .pVertexAttributeDescriptions = attributeDescriptions,
};

PipelineTexExtIyColor::PipelineTexExtIyColor(
    VkDevice device, ShaderStore &shader3D, const RenderOptions &renderOptions,
    VkRenderPass renderPass, VkDescriptorSetLayout desciptorSetLayout)
    : pipelineLayout(device, desciptorSetLayout),
      pipeline(device, shader3D.vertTexExtColor, shader3D.fragTexIyColor,
               vertexInputInfo, renderOptions.scissor, renderPass,
               pipelineLayout, renderOptions.depth, renderOptions.blend,
               renderOptions.polyMode, renderOptions.topology)

{}

PipelineTexExtIyColor::~PipelineTexExtIyColor() {}

void PipelineTexExtIyColor::record(VkCommandBuffer *commandBuffers,
                                   size_t bufferCount,
                                   VkDescriptorSet descriptorSet,
                                   VkBuffer vertices,
                                   VkDeviceSize verticesOffset, VkBuffer uvs,
                                   VkDeviceSize uvOffset, VkBuffer colors,
                                   VkDeviceSize colorOffset, size_t drawCount) {
    if (!drawCount)
        return;

    for (size_t i = 0; i < bufferCount; i++) {

        const VkBuffer buffers[3] = {vertices, uvs, colors};
        VkDeviceSize bufferSizes[3] = {verticesOffset, uvOffset, colorOffset};

        vkCmdBindVertexBuffers(commandBuffers[i], 0, arraySize(buffers),
                               buffers, bufferSizes);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        vkCmdBindDescriptorSets(commandBuffers[i],
                                VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                0, 1, &descriptorSet, 0, nullptr);

        vkCmdDraw(commandBuffers[i], (uint32_t)drawCount, 1, 0, 0);
    }
}

} // namespace BR
