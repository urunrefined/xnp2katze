#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanSampler {
    VkDevice device;

  public:
    VkSampler sampler;

    VulkanSampler(VkPhysicalDevice physicalDevice, VkDevice device,
                  VkFilter filter = VK_FILTER_NEAREST);

    ~VulkanSampler();

    operator VkSampler &() { return sampler; }
};
} // namespace BR
