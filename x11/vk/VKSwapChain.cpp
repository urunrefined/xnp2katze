#include "VKSwapChain.h"
#include "VKUtil.h"

#include "util/Bitset.h"

#include <stdio.h>

#include <array>
#include <limits>
#include <vector>

namespace BR {

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    if (availableFormats.size() == 1 &&
        availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> availablePresentModes) {
    if (availablePresentModes.size() == 0) {
        throw "No presentmode found on card";
    }

    Bitset<4> supported;
    supported.zero();

    std::array<VkPresentModeKHR, 4> priorities{
        VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR};
    const char *priorityStrings[priorities.size()] = {
        "MAILBOX KHR", "RELAXED KHR", "FIFO KHR", "IMMEDIATE KHR"};

    for (const auto &availablePresentMode : availablePresentModes) {
        for (size_t i = 0; i < priorities.size(); i++) {
            if (priorities[i] == availablePresentMode) {
                supported.set(i);

                printf("Supports %s\n", priorityStrings[i]);
            }
        }
    }

    for (size_t i = 0; i < priorities.size(); i++) {
        if (supported(i)) {
            printf("Use %s presentmode\n", priorityStrings[i]);
            return priorities[i];
        }
    }

    throw "No supported presentmode found";
}

static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                                   uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {width, height};

        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

VulkanSwapChain::VulkanSwapChain(VkDevice device,
                                 VkPhysicalDevice physicalDevice,
                                 uint32_t graphicsFamily,
                                 uint32_t presentFamily, VkSurfaceKHR surface,
                                 uint32_t width, uint32_t height,
                                 VulkanSwapChain *oldSwapChain)
    : device(device) {
    SwapChainSupportDetails swapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);

    format = surfaceFormat.format;

    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    printf("Recreate swapchain with dimensions %u %u\n", extent.width,
           extent.height);

    uint32_t queueFamilyIndices[] = {graphicsFamily, presentFamily};

    if (graphicsFamily != presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (oldSwapChain) {
        createInfo.oldSwapchain = oldSwapChain->swapChain;
    } else {
        createInfo.oldSwapchain = nullptr;
    }

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
        VK_SUCCESS) {
        throw "failed to create swap chain!";
    }
}

VulkanSwapChain::~VulkanSwapChain() {
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

} // namespace BR
