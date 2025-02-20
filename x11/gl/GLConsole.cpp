#include <stdint.h>

#include "GLConsole.h"

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
    float marginBottomTop = 0.5f;
    float lineSpacing = 0.1f;
    float spacingInputOutput = 0.6f;

    float marginLeft = 10;

    auto mat = Matrix4x4f::ortho16To9();

    float scale = ((visibleLines.size + 1) * 1);
    scale += ((visibleLines.size - 1) * lineSpacing);
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

void GLConsole::addLine(const LineColor<80> &lineColor) {
    logBuffer.add(lineColor);
}

void GLConsole::up() {
    size_t last = arraySize(logBuffer.lines) - visibleLines.size;

    if (cline + 1 <= last) {
        cline++;
    }
}

void GLConsole::pageUp() {
    size_t last = arraySize(logBuffer.lines) - visibleLines.size;

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
            Vec3 vec[80]{};

            for (size_t c = 0; c < 80; c++) {
                vec[c] = palette.colors[line->colors[c]];
            }

            visibleLines[i].setColor({vec}, arraySize(vec));
        }
    }
}

} // namespace BR
