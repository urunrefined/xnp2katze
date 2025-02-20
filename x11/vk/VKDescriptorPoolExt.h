#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanDescriptorPoolExt {
    VkDevice device;
    VkDescriptorPool descriptorPool;

  public:
    VulkanDescriptorPoolExt(VkDevice device, uint32_t descriptorCount);

    ~VulkanDescriptorPoolExt();

    operator const VkDescriptorPool &() const { return descriptorPool; }
};

} // namespace BR
