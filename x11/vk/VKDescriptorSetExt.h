#pragma once

#include <vulkan/vulkan.h>

#include "VKBuffers.h"
#include "VKPhysicalDevice.h"
#include "gl/GLAlloc.h"
#include "util/Matrix4x4.h"

namespace BR {

class VulkanDescriptorLayoutExt {
    VkDevice device;
    VkDescriptorSetLayout layout;

  public:
    VulkanDescriptorLayoutExt(VkDevice device);
    ~VulkanDescriptorLayoutExt();

    operator const VkDescriptorSetLayout &() const { return layout; }
};

class VulkanDescriptorSetExt {
    VkDevice device;
    VulkanPhysicalDevice physicalDevice;
    VkDescriptorPool descriptorPoolExt;

    std::vector<VkDescriptorSet> descriptorSets;
    UniformChunk chunk;

  private:
    size_t modelCount;

  public:
    VulkanDescriptorSetExt(VkDevice device,
                           VulkanPhysicalDevice &physicalDevice,
                           const UniformChunk &chunk, VkImageView imageView,
                           VkSampler sampler,
                           VkDescriptorPool descriptorPoolExt,
                           VkDescriptorSetLayout layout, size_t modelCount);

    ~VulkanDescriptorSetExt();

    void updateWorldMatrix(const Matrix4x4f &world);
    void updateModelMatrix(const Matrix4x4f &world, size_t index);

    const VkDescriptorSet &get(size_t index) const {
        return descriptorSets[index];
    }

    static size_t getChunkSize(VulkanPhysicalDevice &physicalDevice,
                               uint32_t modelCount);
};

} // namespace BR
