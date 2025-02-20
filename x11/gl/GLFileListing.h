#pragma once

#include "GLText.h"
#include "util/FileListing.h"

#include <dirent.h>

namespace BR {

class GLFileListing {
    size_t cur = 0;
    FileListing fileListing;
    ArrayNTOs<GLTextColors, 32> glListings;
    VulkanDescriptorPoolExt descriptorPoolExt;

  public:
    VulkanDescriptorSetExt descriptorExtSetsMain;

    GLFileListing(const char *directoryName, VkDevice device,
                  VulkanPhysicalDevice &physicalDevice,
                  const UniformChunk &uniformChunk,
                  VulkanImageView &textureView, VulkanSampler &sampler,
                  VulkanDescriptorLayoutExt &descriptorLayoutExt,
                  DataAllocator &dataAllocator)
        : fileListing(directoryName), glListings(dataAllocator, 128),
          descriptorPoolExt(device, 32),
          descriptorExtSetsMain(device, physicalDevice, uniformChunk,
                                textureView, sampler, descriptorPoolExt,
                                descriptorLayoutExt, 32) {

        auto mat = Matrix4x4f::ortho16To9();
        mat.sx() *= 0.1;
        mat.sy() *= 0.1;
        descriptorExtSetsMain.updateWorldMatrix(mat);
    }

    void updateText(std::vector<Mapping> &textGlyphMappingCache,
                    ImageIndexed8 &imageIndexed, FreetypeFace &freetypeFace,
                    HarfbuzzFont &hbfont) {
        for (size_t i = 0;
             i < glListings.size && i < fileListing.filenames.size(); i++) {
            GLTextColors &text = glListings[i];

            printf("%zu File: %s\n", i, fileListing.filenames[i].c_str());

            HarfbuzzText hbText(fileListing.filenames[i].c_str(), hbfont);
            text.setText(hbText, textGlyphMappingCache, imageIndexed,
                         freetypeFace);
            text.setColor({1.0, 0.5, 0.5}, {0.5, 0.2, 1});
        }

        printf("i: %zu\n", glListings.size);

        for (size_t i = 0; i < glListings.size; i++) {
            auto mat = Matrix4x4f::ident();
            descriptorExtSetsMain.updateModelMatrix(mat, i);
        }
    }

    void draw(VulkanRenderer &renderer, VkCommandBuffer *commandBuffers,
              size_t bufferCount) {
        for (size_t i = 0; i < glListings.size; i++) {

            GLTextColors &text = glListings[i];
            renderer.pipelineExtIyColor16to9->record(
                commandBuffers, bufferCount, descriptorExtSetsMain.get(i),
                *text.vtxs.gBuffer, text.vtxs.byteOffset, *text.uvs.gBuffer,
                text.uvs.byteOffset, *text.colors.gBuffer,
                text.colors.byteOffset, text.drawCount);
        }
    }

    ~GLFileListing() {}
};

} // namespace BR
