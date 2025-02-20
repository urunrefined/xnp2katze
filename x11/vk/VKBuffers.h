#pragma once

#include "util/Range.h"
#include <vulkan/vulkan.h>

namespace BR {

class VulkanBufferGeneric {
    VkDevice device;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

  public:
    VkDeviceSize size;

    VulkanBufferGeneric(VkDevice device, VkPhysicalDevice physicalDevice,
                        VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties);
    VulkanBufferGeneric(const VulkanBufferGeneric &) = delete;

    virtual ~VulkanBufferGeneric();

    operator VkBuffer &() { return buffer; }

    operator VkDeviceMemory &() { return bufferMemory; }
};

class VulkanCmbBuffer {
    const VkDevice device;

  public:
    VulkanBufferGeneric bufferCard;
    VulkanBufferGeneric stagingBuffer;

    Ranges ranges;

    VulkanCmbBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                    VkBufferUsageFlags usage, size_t size);
    VulkanCmbBuffer(const VulkanCmbBuffer &that) = delete;

    virtual ~VulkanCmbBuffer() {}

    virtual void update(const char *data, size_t begin, size_t updateSize);

    operator VkBuffer &() { return bufferCard; }
};

class VulkanVtxBuffer : public VulkanCmbBuffer {
  public:
    VulkanVtxBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                    size_t size);
    VulkanVtxBuffer(const VulkanVtxBuffer &that) = delete;
};

class VulkanUniformBuffer : public VulkanCmbBuffer {
  public:
    VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                        size_t size);
    VulkanUniformBuffer(const VulkanVtxBuffer &that) = delete;
};

} // namespace BR
