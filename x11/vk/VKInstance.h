#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanInstance {
  public:
    VkInstance instance;

    VulkanInstance(bool enableValidationLayers);

    ~VulkanInstance();

    operator VkInstance &() { return instance; }
};

} // namespace BR

