#pragma once

#include "VKDescriptorSet.h"
#include "VKDescriptorSetExt.h"
#include "VKDevice.h"

namespace BR {

class VulkanDescriptorLayouts {
  public:
    VulkanDescriptorLayout descriptorLayout;
    VulkanDescriptorLayoutExt descriptorLayoutExt;

    VulkanDescriptorLayouts(VkDevice device);
};

} // namespace BR
