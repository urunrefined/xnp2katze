#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanDescriptorLayout {
    VkDevice device;
    VkDescriptorSetLayout layout;

  public:
    VulkanDescriptorLayout(VkDevice device);
    ~VulkanDescriptorLayout();

    operator const VkDescriptorSetLayout &() const { return layout; }
};

class VulkanDescriptorSet {
    VkDevice device;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

  public:
    VulkanDescriptorSet(VkDevice device, VkImageView imageView,
                        VkSampler sampler, VkDescriptorPool descriptorPool,
                        VkDescriptorSetLayout layout);

    ~VulkanDescriptorSet();

    operator const VkDescriptorSet &() const { return descriptorSet; }
};

} // namespace BR
