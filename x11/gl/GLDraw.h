#pragma once

#include "gl/GLText.h"
#include "vk/VKDescriptorSetExt.h"
#include "vk/VKPipelineTexExtIyColor.h"

namespace BR {

void draw(PipelineTexExtIyColor &pipeline, GLTextColors &text, int setIdx,
          VulkanDescriptorSetExt &descriptorSet,
          VkCommandBuffer *commandBuffers, size_t bufferCount);

}
