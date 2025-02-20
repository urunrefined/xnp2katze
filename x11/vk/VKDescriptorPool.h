#pragma once

#include <vulkan/vulkan.h>

namespace BR {
class VulkanDescriptorPool {
    VkDevice device;
    VkDescriptorPool descriptorPool;

  public:
    VulkanDescriptorPool(VkDevice device, uint32_t descriptorCount);
    ~VulkanDescriptorPool();

    operator const VkDescriptorPool &() const { return descriptorPool; }
};

} // namespace BR
