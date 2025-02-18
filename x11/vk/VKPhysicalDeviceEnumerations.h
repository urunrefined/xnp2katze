#pragma once

#include "VKPhysicalDevice.h"

namespace BR {

class VulkanPhysicalDeviceEnumerations {
  public:
    std::vector<VulkanPhysicalDevice> physicalDevices;

    VulkanPhysicalDeviceEnumerations(VkInstance &instance);
};

} // namespace BR
