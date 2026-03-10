
#include "GLDraw.h"
#include "gl/GLText.h"
#include "vk/VKDescriptorSetExt.h"
#include "vk/VKPipelineTexExtIyColor.h"
#include <cstddef>
#include <vulkan/vulkan_core.h>

namespace BR {

void draw(PipelineTexExtIyColor &pipeline, GLTextColors &text, int setIdx,
          VulkanDescriptorSetExt &descriptorSet,
          VkCommandBuffer *commandBuffers, size_t bufferCount) {

    pipeline.record(commandBuffers, bufferCount, descriptorSet.get(setIdx),
                    *text.vtxs.gBuffer, text.vtxs.byteOffset, *text.uvs.gBuffer,
                    text.uvs.byteOffset, *text.colors.gBuffer,
                    text.colors.byteOffset, text.drawCount);
}

} // namespace BR
