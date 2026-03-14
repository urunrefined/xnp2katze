#include <cstddef>
#include <stdint.h>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GLConsole.h"
#include "font/FontContext.h"
#include "font/Harfbuzz.h"
#include "gl/GLAlloc.h"
#include "util/Core.h"
#include "util/LineColor.h"
#include "util/Matrix4x4.h"
#include "vk/VKDescriptorLayouts.h"
#include "vk/VKPhysicalDevice.h"
#include "vk/VKSampler.h"

namespace BR {

GLConsole::GLConsole(DataAllocator &dataAllocator, FontContext &fontContext,
                     size_t maxGlyphs, VkDevice device,
                     VulkanPhysicalDevice &physicalDevice, UniformAllocator &ua,
                     VulkanSampler &sampler, VulkanDescriptorLayouts &layouts,
                     VkImageView imageView)
    : commandLine(dataAllocator, maxGlyphs), palette({1, 1, 1}),
      visibleLines(dataAllocator, maxGlyphs), fontContext(fontContext),
      pool(device, visibleLines.size + 1),
      sets(device, physicalDevice,
           ua.getChunk(physicalDevice.getMinUniformBufferOffsetAlignment(),
                       VulkanDescriptorSetExt::getChunkSize(
                           physicalDevice, visibleLines.size + 1)),
           imageView, sampler, pool, layouts.descriptorLayoutExt,
           visibleLines.size + 1)

{
    const float marginBottomTop = 0.5f;
    const float lineSpacing = 0.1f;
    const float spacingInputOutput = 0.6f;

    const float marginLeft = 10;

    auto mat = Matrix4x4f::ortho(4.f / 3.f);

    float scale = (float)((visibleLines.size + 1) * 1);
    scale += (float)((float)(visibleLines.size - 1) * lineSpacing);
    scale += marginBottomTop * 2;
    scale += spacingInputOutput;

    scale = (1.0f / scale);

    mat.sx() *= (scale);
    mat.sy() *= (scale);

    sets.updateWorldMatrix(mat);

    float curY = marginBottomTop;

    palette.colors[0] = {1, 1, 1};
    palette.colors[1] = {0.2, 0.4, 1};
    palette.colors[2] = {0.7, 0.2, 0.1};

    {
        auto mat = Matrix4x4f::ident();
        mat.tx() = marginLeft;
        mat.ty() = curY;

        sets.updateModelMatrix(mat, 0);

        curY += 1.0f;
    }

    curY += spacingInputOutput;

    for (size_t i = 1; i < visibleLines.size + 1; i++) {
        auto mat = Matrix4x4f::ident();
        mat.tx() = marginLeft;
        mat.ty() = curY;

        sets.updateModelMatrix(mat, i);

        curY += lineSpacing + 1.0f;
    }
}

void GLConsole::add(const std::vector<uint32_t> &addPoints) {

    codePoints.insert(codePoints.end(), addPoints.begin(), addPoints.end());
    HarfbuzzText text(codePoints, fontContext.harfbuzzFont);

    commandLine.setText(text, fontContext.textGlyphMappingCache,
                        fontContext.imageIndexed, fontContext.freetypeFace);
}

void GLConsole::del() {
    if (!codePoints.empty()) {
        codePoints.pop_back();
        HarfbuzzText text(codePoints, fontContext.harfbuzzFont);

        commandLine.setText(text, fontContext.textGlyphMappingCache,
                            fontContext.imageIndexed, fontContext.freetypeFace);
    }
}

void GLConsole::clear() {
    if (!codePoints.empty()) {
        codePoints.clear();

        HarfbuzzText text(codePoints, fontContext.harfbuzzFont);
        commandLine.setText(text, fontContext.textGlyphMappingCache,
                            fontContext.imageIndexed, fontContext.freetypeFace);
    }
}

void GLConsole::addLine(const std::string &line) {
    logBuffer.add(line.c_str());
}

void GLConsole::addLine(const LineColor<132> &lineColor) {
    logBuffer.add(lineColor);
}

void GLConsole::up() {
    const size_t last = arraySize(logBuffer.lines) - visibleLines.size;

    if (cline + 1 <= last) {
        cline++;
    }
}

void GLConsole::pageUp() {
    const size_t last = arraySize(logBuffer.lines) - visibleLines.size;

    if (cline + visibleLines.size <= last) {
        cline += visibleLines.size;
    } else {
        cline = last;
    }
}

void GLConsole::down() {
    if (cline) {
        cline--;
    }
}

void GLConsole::pageDown() {
    if (cline >= visibleLines.size) {
        cline -= visibleLines.size;
    } else {
        cline = 0;
    }
}

void GLConsole::ready() {
    for (size_t i = 0; i < visibleLines.size; i++) {
        auto *line = logBuffer.get(cline + i);

        HarfbuzzText text(line->text, fontContext.harfbuzzFont);

        visibleLines[i].setText(text, fontContext.textGlyphMappingCache,
                                fontContext.imageIndexed,
                                fontContext.freetypeFace);

        {
            Vec3 vec[132]{};

            for (size_t c = 0; c < 132; c++) {
                vec[c] = palette.colors[line->colors[c]];
            }

            visibleLines[i].setColor(vec, arraySize(vec));
        }
    }
}

} // namespace BR
