#include <stdint.h>

#include "GLText.h"
#include <algorithm>

namespace BR {

GLText::GLText(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
               HarfbuzzText &text, std::vector<Mapping> &textGlyphMappingCache,
               ImageIndexed8 &imageIndexed, FreetypeFace &freetypeFace)
    : vtxs(device, physicalDevice, text.getGlyphCount() * sizeof(Vec2) * 6),
      uvs(device, physicalDevice, text.getGlyphCount() * sizeof(Vec2) * 6),
      drawCount(text.getGlyphCount() * 6) {

    auto textMappings = getTextMappings(imageIndexed, textGlyphMappingCache,
                                        freetypeFace, text);

    /*
      printf("-- mappingCache: %zu, textMapping: %zu\n",
             textGlyphMappingCache.size(), textMappings.size());
    */
    std::vector<Vec2> dux;
    std::vector<Vec2> duv;

    mappingsToGlData(freetypeFace.pxSize, textMappings, dux, duv);

    /*  printf("== GL DATA %zu %zu\n", dux.size(), duv.size());*/

    vtxs.update((const char *)dux.data(), 0, dux.size() * sizeof(Vec2));
    uvs.update((const char *)duv.data(), 0, duv.size() * sizeof(Vec2));
}

GLTextColors::GLTextColors(DataAllocator &dataAllocator, HarfbuzzText &text,
                           std::vector<Mapping> &textGlyphMappingCache,
                           ImageIndexed8 &imageIndexed,
                           FreetypeFace &freetypeFace)
    : vtxs(dataAllocator.get2fSegment(text.getGlyphCount() * 6)),
      uvs(dataAllocator.get2fSegment(text.getGlyphCount() * 6)),
      colors(dataAllocator.get3fSegment(text.getGlyphCount() * 6)),
      drawCount(text.getGlyphCount() * 6), maxDrawCount(drawCount) {

    setText(text, textGlyphMappingCache, imageIndexed, freetypeFace);
    setColor({1, 1, 1});
}

GLTextColors::GLTextColors(DataAllocator &dataAllocator, size_t maxGlyphs)
    : vtxs(dataAllocator.get2fSegment(maxGlyphs * 6)),
      uvs(dataAllocator.get2fSegment(maxGlyphs * 6)),
      colors(dataAllocator.get3fSegment(maxGlyphs * 6)), drawCount(0),
      maxDrawCount(maxGlyphs * 6) {}

void GLTextColors::setText(HarfbuzzText &text,
                           std::vector<Mapping> &textGlyphMappingCache,
                           ImageIndexed8 &imageIndexed,
                           FreetypeFace &freetypeFace) {

    drawCount =
        std::min((uint32_t)maxDrawCount, (uint32_t)(text.getGlyphCount() * 6));

    auto textMappings = getTextMappings(imageIndexed, textGlyphMappingCache,
                                        freetypeFace, text);

    std::vector<Vec2> dux;
    std::vector<Vec2> duv;

    mappingsToGlData(freetypeFace.pxSize, textMappings, dux, duv);

    setColor({1, 1, 1});

    if (dux.size() > drawCount)
        dux.resize(drawCount);

    if (duv.size() > drawCount)
        duv.resize(drawCount);

    vtxs.update(ArrayView<Vec2>(dux.data(), dux.size()));
    uvs.update(ArrayView<Vec2>(duv.data(), duv.size()));
}

void GLTextColors::setColor(const Vec3 &color) {
    std::vector<Vec3> duc;
    duc.reserve(drawCount);

    for (uint32_t i = 0; i < drawCount; i++) {
        duc.push_back(color);
    }

    colors.update(ArrayView<Vec3>(duc.data(), duc.size()));
}

void GLTextColors::setColor(const Vec3 &colorA, const Vec3 &colorB) {
    int32_t letterCount = drawCount / 6;

    // printf("DrawCount %d\n", drawCount);

    if (!letterCount)
        return;

    std::vector<Vec3> duc;
    duc.reserve(letterCount * 6);

    if (letterCount == 1) {
        duc.push_back(colorA);
        duc.push_back(colorA);
        duc.push_back(colorA);
        duc.push_back(colorA);
        duc.push_back(colorA);
        duc.push_back(colorA);
    } else {

        Vec3 step = (colorB - colorA) / (letterCount - 1);
        Vec3 start = colorA;

        for (int32_t i = 0; i < letterCount; i++) {
            duc.push_back(start);
            duc.push_back(start);
            duc.push_back(start);
            duc.push_back(start);
            duc.push_back(start);
            duc.push_back(start);

            // start.print();

            start += step;
        }
    }

    colors.update(ArrayView<Vec3>(duc.data(), duc.size()));
}

void GLTextColors::setColor(const Vec3 *newColors, size_t count) {
    size_t letterCount = drawCount / 6;

    std::vector<Vec3> duc;
    letterCount = std::min(letterCount, count);

    duc.reserve(letterCount * 6);

    for (size_t i = 0; i < letterCount; i++) {
        duc.push_back(newColors[i]);
        duc.push_back(newColors[i]);
        duc.push_back(newColors[i]);
        duc.push_back(newColors[i]);
        duc.push_back(newColors[i]);
        duc.push_back(newColors[i]);
    }

    colors.update(ArrayView<Vec3>(duc.data(), duc.size()));
}

} // namespace BR
