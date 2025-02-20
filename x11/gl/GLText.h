#pragma once

#include <stdint.h>
#include <vector>

#include "font/FreeFont.h"
#include "gl/GLAlloc.h"
#include "util/LineColor.h"
#include "util/Palette.h"
#include "vk/VKBuffers.h"
#include "vk/VKDevice.h"
#include "vk/VKPhysicalDevice.h"

namespace BR {

class GLText {
  public:
    VulkanVtxBuffer vtxs;
    VulkanVtxBuffer uvs;
    uint32_t drawCount = 0;

    GLText(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
           HarfbuzzText &text, std::vector<Mapping> &textGlyphMappingCache,
           ImageIndexed8 &imageIndexed, FreetypeFace &freetypeFace);
};

class GLTextColors {
  public:
    Segment2f vtxs;
    Segment2f uvs;
    Segment3f colors;

    uint32_t drawCount = 0;
    uint32_t maxDrawCount = 0;

    GLTextColors(DataAllocator &dataAllocator, HarfbuzzText &text,
                 std::vector<Mapping> &textGlyphMappingCache,
                 ImageIndexed8 &imageIndexed, FreetypeFace &freetypeFace);

    GLTextColors(DataAllocator &dataAllocator, size_t maxGlyphs);

    void setText(HarfbuzzText &text,
                 std::vector<Mapping> &textGlyphMappingCache,
                 ImageIndexed8 &imageIndexed, FreetypeFace &freetypeFace);

    void setColor(const Vec3 &color);
    void setColor(const Vec3 &colorA, const Vec3 &colorB);
    void setColor(const Vec3 *newColors, size_t count);
};

} // namespace BR
