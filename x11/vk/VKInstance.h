#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace BR {

class VulkanInstance {
  public:
    VkInstance instance;

    VulkanInstance(std::vector<const char *> &extensions,
                   std::vector<const char *> &layers);

    ~VulkanInstance();

    operator VkInstance &() { return instance; }
};

} // namespace BR
