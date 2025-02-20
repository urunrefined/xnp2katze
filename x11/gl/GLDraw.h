#pragma once

#include "gl/GLText.h"
#include "vk/VKRenderer.h"

namespace BR {

void draw(VulkanRenderer &renderer, GLTextColors &text, int setIdx,
          VulkanDescriptorSetExt &descriptorSet,
          VkCommandBuffer *commandBuffers, size_t bufferCount);

}
