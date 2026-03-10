#pragma once

#include <stdint.h>
#include <vector>

#include "font/FontContext.h"
#include "gl/GLAlloc.h"
#include "gl/GLText.h"
#include "vk/VKBuffers.h"
#include "vk/VKDescriptorLayouts.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/VKDevice.h"
#include "vk/VKPhysicalDevice.h"
#include "vk/VKPipelineTexExtIyColor.h"

#include "vk/VKSampler.h"

#include "util/Core.h"
#include "util/LogBuffer.h"

namespace BR {

class GLConsole {
  public:
    GLTextColors commandLine;
    Palette8f palette;

    ArrayNTOs<GLTextColors, 32> visibleLines;

    std::vector<uint32_t> codePoints;
    FontContext &fontContext;

    size_t cline = 0;
    LogBuffer logBuffer;

    VulkanDescriptorPoolExt pool;
    VulkanDescriptorSetExt sets;

  public:
    GLConsole(DataAllocator &dataAllocator, FontContext &fontContext,
              size_t maxGlyphs, VkDevice device,
              VulkanPhysicalDevice &physicalDevice, UniformAllocator &ua,
              VulkanSampler &sampler, VulkanDescriptorLayouts &layouts,
              VkImageView imageView);

    void add(const std::vector<uint32_t> &addPoints);
    void del();
    void clear();

    void addLine(const std::string &line);
    void addLine(const LineColor<80> &lineColor);

    void draw(PipelineTexExtIyColor &pipeline, VkCommandBuffer *commandBuffers,
              size_t bufferCount) {

        pipeline.record(commandBuffers, bufferCount, sets.get(0),
                        *commandLine.vtxs.gBuffer, commandLine.vtxs.byteOffset,
                        *commandLine.uvs.gBuffer, commandLine.uvs.byteOffset,
                        *commandLine.colors.gBuffer,
                        commandLine.colors.byteOffset, commandLine.drawCount);

        for (size_t i = 0; i < visibleLines.size; i++) {

            const GLTextColors &text = visibleLines[i];
            pipeline.record(commandBuffers, bufferCount, sets.get(i + 1),
                            *text.vtxs.gBuffer, text.vtxs.byteOffset,
                            *text.uvs.gBuffer, text.uvs.byteOffset,
                            *text.colors.gBuffer, text.colors.byteOffset,
                            text.drawCount);
        }
    }

    void ready();

    void up();
    void down();
    void pageUp();
    void pageDown();
};

} // namespace BR
