#include "VKRenderer.h"

#include <string.h>

namespace BR {

VulkanRenderer::VulkanRenderer(VulkanPhysicalDevice &physicalDevice,
                               VkDevice device, ShaderStore &shaderStore,
                               const VkExtent2D &swapChainExtent,
                               VulkanRenderPass &renderPass,
                               VulkanDescriptorLayouts &layouts)
    : physicalDevice(physicalDevice), device(device), shaderStore(shaderStore),
      layouts(layouts), renderPass(renderPass) {
    reCreatePipeline(swapChainExtent);
}

} // namespace BR
