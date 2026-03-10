#include "VKDescriptorLayouts.h"
#include <vulkan/vulkan_core.h>

namespace BR {

VulkanDescriptorLayouts::VulkanDescriptorLayouts(VkDevice device)
    : descriptorLayout(device), descriptorLayoutExt(device) {}

} // namespace BR
