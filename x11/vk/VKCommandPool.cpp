#include "VKCommandPool.h"

namespace BR {

VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t graphicsFamily)
    : device(device) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsFamily;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        throw "failed to create graphics command pool!";
    }
}

VulkanCommandPool::~VulkanCommandPool() {
    vkDestroyCommandPool(device, commandPool, nullptr);
}

} // namespace BR
