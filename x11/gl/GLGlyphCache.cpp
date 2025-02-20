#include "GLGlyphCache.h"

namespace BR {

GlyphCache::GlyphCache(VulkanPhysicalDevice &physicalDevice, VkDevice device)
    : alphaTexture(device, physicalDevice, 1024, 1024),
      imageIndexed({alphaTexture, 0, false}) {
    fill(alphaTexture);
}

} // namespace BR
