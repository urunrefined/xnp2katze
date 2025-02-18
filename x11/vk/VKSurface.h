#pragma once

#include "VKGLFW.h"

namespace BR {

class VulkanSurface {
  public:
    VkInstance &instance;
    VkSurfaceKHR surface;

    VulkanSurface(GLFWContext &context, VkInstance &instance_);
    ~VulkanSurface();

    operator VkSurfaceKHR &() { return surface; }
};

} // namespace BR
