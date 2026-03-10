#include "VKDevice.h"

#include "VKUtil.h"
#include <cstdint>

#include <set>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace BR {

static const std::vector<const char *> validationLayers = {
    //"VK_LAYER_LUNARG_vktrace",
    //    "VK_LAYER_MESA_overlay",
    "VK_LAYER_KHRONOS_validation"};

VulkanDevice::VulkanDevice(
    bool enableValidationLayer, VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    const std::vector<VkQueueFamilyProperties> &queueFamilies) {

    const uint32_t presentIdx =
        getFirstPresentQueue(physicalDevice, surface, queueFamilies);
    const uint32_t graphicsIdx =
        getFirstGraphicsQueue(physicalDevice, surface, queueFamilies);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const std::set<uint32_t> uniqueQueueFamilies = {graphicsIdx, presentIdx};

    const float queuePriority = 1.0f;
    for (auto queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    createInfo.pEnabledFeatures = &deviceFeatures;

    const char *const extensions[]{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    createInfo.enabledExtensionCount =
        sizeof(extensions) / sizeof(extensions[0]);
    createInfo.ppEnabledExtensionNames = extensions;

    if (enableValidationLayer) {
        createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
        throw "failed to create logical device!";
    }

    vkGetDeviceQueue(device, graphicsIdx, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentIdx, 0, &presentQueue);

    graphicsFamily = graphicsIdx;
    presentFamily = presentIdx;
}

VulkanDevice::~VulkanDevice() {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
}

VulkanPureDevice::VulkanPureDevice(
    bool enableValidationLayer, VkPhysicalDevice physicalDevice,
    const std::vector<VkQueueFamilyProperties> &queueFamilies) {

    const uint32_t graphicsIdx = getFirstGraphicsQueue(queueFamilies);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const std::set<uint32_t> uniqueQueueFamilies = {graphicsIdx};

    const float queuePriority = 1.0f;
    for (const auto queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = 0;

    if (enableValidationLayer) {
        createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
        throw "failed to create logical device!";
    }

    graphicsQueue = 0;
    vkGetDeviceQueue(device, graphicsIdx, 0, &graphicsQueue);
    graphicsFamily = graphicsIdx;
}

VulkanPureDevice::~VulkanPureDevice() {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
}

} // namespace BR
