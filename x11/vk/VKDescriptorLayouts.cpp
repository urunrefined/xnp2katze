#include "VKDescriptorLayouts.h"

namespace BR {

VulkanDescriptorLayouts::VulkanDescriptorLayouts(VkDevice device)
    : descriptorLayout(device), descriptorLayoutExt(device) {}

} // namespace BR
