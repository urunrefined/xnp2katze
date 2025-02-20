
#include "GLDraw.h"

namespace BR {

void draw(VulkanRenderer &renderer, GLTextColors &text, int setIdx,
          VulkanDescriptorSetExt &descriptorSet,
          VkCommandBuffer *commandBuffers, size_t bufferCount) {

    renderer.pipelineExtIyColor16to9->record(
        commandBuffers, bufferCount, descriptorSet.get(setIdx),
        *text.vtxs.gBuffer, text.vtxs.byteOffset, *text.uvs.gBuffer,
        text.uvs.byteOffset, *text.colors.gBuffer, text.colors.byteOffset,
        text.drawCount);
}

} // namespace BR
