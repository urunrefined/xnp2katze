#include "VKImageMemory.h"
#include "vk/VKUtil.h"
#include <vulkan/vulkan_core.h>

namespace BR {

VulkanImageMemory::VulkanImageMemory(VkPhysicalDevice physicalDevice,
                                     VkDevice device, VkImage image,
                                     VkMemoryPropertyFlags properties)
    : device(device) {
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
        VK_SUCCESS) {
        throw "failed to allocate image memory!";
    }

    if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS) {
        vkFreeMemory(device, imageMemory, 0);
        throw "failed to bind image memory!";
    }
}

VulkanImageMemory::~VulkanImageMemory() {
    vkFreeMemory(device, imageMemory, 0);
}

} // namespace BR
