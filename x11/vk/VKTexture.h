#pragma once

#include "VKBuffers.h"
#include "VKCommandBuffer.h"
#include "VKCommandPool.h"
#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKImageView.h"

#include "util/Enums.h"
#include "util/Image.h"

#include <stdio.h>
#include <vector>

namespace BR {

unsigned int getFormatComponents(VkFormat format);

class VulkanTextureGeneric {
  protected:
    uint16_t width;
    uint16_t height;
    unsigned int components;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VulkanImage texture;
    VulkanImageMemory textureMemory;

  public:
    VulkanImageView textureView;

  private:
    bool textureDirty;

  public:
    VulkanBufferGeneric stagingBuffer;
    void *data;

    VulkanTextureGeneric(const VkDevice device,
                         const VkPhysicalDevice physicalDevice, uint16_t width,
                         uint16_t height, VkFormat format,
                         VkBufferUsageFlagBits hostUsage,
                         VkImageUsageFlagBits deviceUsage)
        :

          width(width), height(height), components(getFormatComponents(format)),
          device(device), physicalDevice(physicalDevice),
          texture(device, width, height, format, deviceUsage),
          textureMemory(physicalDevice, device, texture,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),

          textureView(device, texture, format, VK_IMAGE_ASPECT_COLOR_BIT),
          textureDirty(true),
          stagingBuffer(device, physicalDevice, width * height * components,
                        hostUsage,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)

    {

        vkMapMemory(device, stagingBuffer, 0, width * height * components, 0,
                    &data);
    }

    ~VulkanTextureGeneric() { vkUnmapMemory(device, stagingBuffer); }

    void update(VulkanCommandBuffer &buffer);
    void fetch(VulkanCommandBuffer &buffer);

    void dirty() { textureDirty = true; }
    void clean() { textureDirty = false; }
};

class VulkanTextureBGRA : public VulkanTextureGeneric {
  public:
    VulkanTextureBGRA(const VkDevice device,
                      const VkPhysicalDevice physicalDevice, uint16_t width,
                      uint16_t height)
        : VulkanTextureGeneric(
              device, physicalDevice, width, height, VK_FORMAT_B8G8R8A8_UNORM,
              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
              (VkImageUsageFlagBits)(VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                     VK_IMAGE_USAGE_SAMPLED_BIT)) {}

    operator ImageBGRA() { return ImageBGRA{width, height, (char *)data}; }
};

class VulkanTextureBGRARender : public VulkanTextureGeneric {
  public:
    VulkanTextureBGRARender(const VkDevice device,
                            const VkPhysicalDevice physicalDevice,
                            uint16_t width, uint16_t height)
        : VulkanTextureGeneric(
              device, physicalDevice, width, height, VK_FORMAT_B8G8R8A8_UNORM,
              (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT),
              (VkImageUsageFlagBits)(

                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |

                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT)) {}

    operator ImageBGRA() { return ImageBGRA{width, height, (char *)data}; }
};

class VulkanTexture8 : public VulkanTextureGeneric {
  public:
    VulkanTexture8(const VkDevice device, const VkPhysicalDevice physicalDevice,
                   uint16_t width, uint16_t height)
        : VulkanTextureGeneric(
              device, physicalDevice, width, height, VK_FORMAT_R8_UNORM,
              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
              (VkImageUsageFlagBits)(VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                     VK_IMAGE_USAGE_SAMPLED_BIT)) {}

    operator Image8() { return Image8{width, height, (char *)data}; }
};

} // namespace BR
