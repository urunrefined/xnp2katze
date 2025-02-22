#pragma once

#include <vulkan/vulkan.h>

struct RenderOptions {
    VkBool32 depth;
    VkBool32 blend;

    VkPrimitiveTopology topology;
    VkPolygonMode polyMode;
    VkRect2D scissor;
};
