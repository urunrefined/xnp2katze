#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

namespace BR {

class VulkanSampler {
    const VkDevice &device;

  public:
    VkSampler sampler;

    VulkanSampler(const VkPhysicalDevice &physicalDevice,
                  const VkDevice &device_);

    ~VulkanSampler();

    operator VkSampler &() { return sampler; }
};
} // namespace BR

