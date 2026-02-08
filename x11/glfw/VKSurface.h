#pragma once

#include "VKGLFW.h"

namespace BR {

class VulkanSurface {
  public:
    VkInstance &instance;
    VkSurfaceKHR surface;

    VulkanSurface(GLFWSurface &glfwSurface, VkInstance &instance_);
    ~VulkanSurface();

    operator VkSurfaceKHR &() { return surface; }
};

} // namespace BR
