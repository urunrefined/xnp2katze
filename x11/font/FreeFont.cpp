
#include "FreeFont.h"
#include "Fontconfig.h"
#include "font/Freetype.h"
#include "font/Harfbuzz.h"
#include "util/Image.h"
#include "util/Vertex.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <freetype/freetype.h>
#include <freetype/ftimage.h>
#include <freetype/fttypes.h>
#include <hb.h>
#include <string>
#include <vector>

namespace BR {

static void draw_bitmapAlpha(FT_Bitmap *bitmap, Image8 &image, uint32_t xOffset,
                             uint32_t yOffset, unsigned int pxSize,
                             int leftOffset) {
    if (!bitmap->rows)
        return;

    uint8_t *data = image.data;

    // bitmap->rows /width could theoretically be larger than pxSize, so cut it
    // off here

    const unsigned int rows = std::min(pxSize, bitmap->rows);
    const unsigned int width = std::min(pxSize, bitmap->width);

    for (unsigned int y = 0; y < rows; y++) {
        const size_t row = ((size_t)y + yOffset) * image.width;

        for (unsigned int x = 0; x < width; x++) {

            if (leftOffset + x > pxSize)
                continue;

            const unsigned int col = xOffset + x;

            const size_t idx = row + col + leftOffset;

            if (idx >= image.byteSize())
                continue;

            const uint8_t px =
                bitmap->buffer[bitmap->width * (bitmap->rows - 1 - y) + (x)];
            data[idx] |= px;
        }
    }
}

static ImageGlyph toImageGlyph(float penX, float penY, uint32_t idx,
                               Image8 &image) {

    //  printf("Glyph %u, %u, %d, %u\n", penX, penY, descender, idx);

    return {penX, penY, idx, image};
}

static void get2DSquareBox(float startX, float startY, float wh,
                           std::vector<Vec2> &ret) {
    // clang-format off

  Vec2 const lu {float(startX)       , float(startY + wh)};
  Vec2 const lb {float(startX)       , float(startY)     };
  Vec2 const ru {float(startX + (wh )), float(startY + wh)};
  Vec2 const rb {float(startX + (wh )), float(startY)     };

    // clang-format on

    ret.push_back(lu);
    ret.push_back(lb);
    ret.push_back(rb);

    ret.push_back(lu);
    ret.push_back(rb);
    ret.push_back(ru);
}

static void getUVs(std::vector<Vec2> &uvs, float x, float y, float xSize,
                   float ySize, float imageWidth, float imageHeight) {
    // clang-format off
  
//  printf("--------- x %f, y %f, xSize %f, ySize %f\n", x, y, xSize, ySize);

  Vec2 const lb { x                 / imageWidth,  y           / imageHeight };
  Vec2 const lu { x                 / imageWidth, (y + ySize - 0.5f)  / imageHeight };
  Vec2 const rb {(x + xSize - 0.5f) / imageWidth,  y           / imageHeight };
  Vec2 const ru {(x + xSize - 0.5f) / imageWidth, (y + ySize - 0.5f)  / imageHeight };

  /*
  printf("lb %f %f\n", lb.x, lb.y);
  printf("lu %f %f\n", lu.x, lu.y);
  printf("rb %f %f\n", rb.x, rb.y);
  printf("ru %f %f\n", ru.x, ru.y);
  */

    // clang-format on

    uvs.push_back(lu);
    uvs.push_back(lb);
    uvs.push_back(rb);

    uvs.push_back(lu);
    uvs.push_back(rb);
    uvs.push_back(ru);
}

void mappingsToGlData(unsigned int pxSize, std::vector<ImageGlyph> &imageGlyphs,
                      std::vector<Vec2> &vtxs, std::vector<Vec2> &uvs) {

    for (const auto &imageGlyph : imageGlyphs) {
        const uint16_t glyphsPerLine = imageGlyph.image.width / pxSize;

        const uint32_t wSlot = imageGlyph.idx % glyphsPerLine;
        const uint32_t hSlot = imageGlyph.idx / glyphsPerLine;

        get2DSquareBox(imageGlyph.offsetX, imageGlyph.offsetY, 1, vtxs);

        getUVs(uvs, (float)(wSlot * pxSize), (float)(hSlot * pxSize),
               (float)pxSize, (float)pxSize, (float)imageGlyph.image.width,
               (float)imageGlyph.image.height);
    }
}

std::vector<ImageGlyph> getTextMappings(ImageIndexed8 &image,
                                        std::vector<Mapping> &glyphCache,
                                        FreetypeFace &face,
                                        HarfbuzzText &text) {
    //  text.print();

    std::vector<ImageGlyph> textGlyphs;
    textGlyphs.reserve(text.getGlyphCount());

    float penX = 0;
    float penY = 0;

    for (unsigned int n = 0; n < text.getGlyphCount(); n++) {
        const hb_codepoint_t codepoint = text.getGlyph(n);

        //    printf("offsetX %d, offsetY: %d\n", text.getOffsetX(n),
        //    text.getOffsetY(n));

        auto gl = std::find(glyphCache.begin(), glyphCache.end(), codepoint);

        if (gl == glyphCache.end()) {
            const uint16_t glyphsPerLine = image.image.width / face.pxSize;

            const uint32_t wslot = image.idx % glyphsPerLine;
            const uint32_t hslot = image.idx / glyphsPerLine;

            //      printf("New Glyph on slot %d %d, idx %d\n", wslot, hslot,
            //      image.idx);

            const FT_Error error =
                FT_Load_Glyph(face.face, codepoint, FT_LOAD_RENDER);

            if (error) {
                printf("Load glyph failed for %d\n", codepoint);
            } else {

                FT_GlyphSlot &slot = face.slot;
                /*
                        printf("face.lineheight %d\n", face.lineheight);
                        printf("face.descender %d\n", face.descender);

                        printf("face.height %ld\n", slot->metrics.height);
                        printf("face.bearing %ld\n",
                   slot->metrics.vertBearingY);
                */

                /*
                int ladj = (slot->metrics.height - slot->metrics.horiBearingX) *
                           (64 / face.pxSize);

                int uadj = (slot->metrics.height - slot->metrics.horiBearingY) *
                          (64 / face.pxSize);
                */

                //        printf("Diff %ld\n", diffH);
                /*
                printf("--Metrics [top:%d, left:%d] [hx:%d, d:%d]\n",
                slot->bitmap_top, slot->bitmap_left, ladj, uadj);
                */

                const auto &metrics = slot->metrics;
                /*
                        printf("Metrics width [%ld/%ld] height [%ld/%ld]\n",
                   metrics.width, metrics.width / 64, metrics.height,
                   metrics.height / 64);

                        printf("Metrics horiX [%ld/%ld] horiY [%ld/%ld]\n",
                               metrics.horiBearingX, metrics.horiBearingX / 64,
                               metrics.horiBearingY, metrics.horiBearingY / 64);
                */

                const float descender =
                    (float)(metrics.height - metrics.horiBearingY) / 64.0f /
                    (float)face.pxSize;

                draw_bitmapAlpha(&slot->bitmap, image.image,
                                 wslot * face.pxSize, (hslot * face.pxSize),
                                 face.pxSize, slot->bitmap_left);
                // slot->bitmap_left, slot->bitmap_top);

                glyphCache.push_back(
                    {codepoint, image.idx, descender, image.image});

                /*
                hb_glyph_extents_t extents;
                hb_font_get_glyph_extents(font.font, codepoint, &extents);
                */

                textGlyphs.push_back(toImageGlyph(penX, penY - descender,
                                                  image.idx, image.image));

                /*
                        printf("y: %d, height: %d   ---=--= %d\n",
                          extents.y_bearing, extents.height, extents.height +
                   extents.y_bearing);
                */

                image.dirty = true;
                image.idx++;
            }
        } else {
            textGlyphs.push_back(
                toImageGlyph(penX, penY - gl->descender, gl->idx, image.image));
        }

        penX += ((float)text.getAdvanceX(n)) / 64.0f / (float)face.pxSize;
        penY += ((float)text.getAdvanceY(n)) / 64.0f / (float)face.pxSize;
    }

    return textGlyphs;
}

std::string getFont() {
    FontconfigLib lib;
    FontList fontlist(lib);

    std::vector<std::string> fontFiles = fontlist.getFontList({});

    for (auto &str : fontFiles) {
        printf("--%s--\n", str.c_str());
    }

    if (fontFiles.empty()) {
        return "";
    }

    return fontFiles.at(0);
}

} // namespace BR
