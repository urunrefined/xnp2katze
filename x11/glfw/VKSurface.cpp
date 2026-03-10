#include "VKSurface.h"
#include "glfw/VKGLFW.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace BR {

VulkanSurface::VulkanSurface(GLFWSurface &glfwSurface, VkInstance &instance_)
    : instance(instance_) {
    if (glfwCreateWindowSurface(instance, glfwSurface.window, nullptr,
                                &surface) != VK_SUCCESS) {
        throw "failed to create window surface!";
    }
}

VulkanSurface::~VulkanSurface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

} // namespace BR
