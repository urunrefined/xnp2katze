#pragma once

#include "font/FreeFont.h"
#include "util/Deep.h"
#include "vk/VKDevice.h"
#include "vk/VKPhysicalDevice.h"
#include "vk/VKTexture.h"

namespace BR {

class GlyphCache {
  public:
    VulkanTexture8 alphaTexture;
    ImageIndexed8 imageIndexed;
    std::vector<Mapping> textGlyphMappingCache;

    GlyphCache(VulkanPhysicalDevice &physicalDevice, VkDevice device);
};

} // namespace BR
