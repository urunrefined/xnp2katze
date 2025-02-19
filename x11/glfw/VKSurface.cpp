#include "VKSurface.h"

namespace BR {

VulkanSurface::VulkanSurface(GLFWContext &context, VkInstance &instance_)
    : instance(instance_) {
    if (glfwCreateWindowSurface(instance, context.window, nullptr, &surface) !=
        VK_SUCCESS) {
        throw "failed to create window surface!";
    }
}

VulkanSurface::~VulkanSurface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

} // namespace BR
