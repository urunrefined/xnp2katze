
#include "FreeFont.h"
#include "Fontconfig.h"

#include <algorithm>

namespace BR {

static void draw_bitmapAlpha(FT_Bitmap *bitmap, Image8 &image, int xOffset,
                             int yOffset, unsigned int pxSize, int leftOffset) {
    if (!bitmap->rows)
        return;

    char *data = image.data;

    // bitmap->rows /width could theoretically be larger than pxSize, so cut it
    // off here

    unsigned int rows = std::min(pxSize, bitmap->rows);
    unsigned int width = std::min(pxSize, bitmap->width);

    for (unsigned int y = 0; y < rows; y++) {
        size_t row = (y + yOffset) * image.width;

        for (unsigned int x = 0; x < width; x++) {

            if (leftOffset + x > pxSize)
                continue;

            unsigned int col = xOffset + x;

            size_t idx = row + col + leftOffset;

            if (idx >= image.byteSize())
                continue;

            uint8_t px =
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

  Vec2 lu {float(startX)       , float(startY + wh)};
  Vec2 lb {float(startX)       , float(startY)     };
  Vec2 ru {float(startX + (wh )), float(startY + wh)};
  Vec2 rb {float(startX + (wh )), float(startY)     };

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

  Vec2 lb { x                 / imageWidth,  y           / imageHeight };
  Vec2 lu { x                 / imageWidth, (y + ySize - 0.5f)  / imageHeight };
  Vec2 rb {(x + xSize - 0.5f) / imageWidth,  y           / imageHeight };
  Vec2 ru {(x + xSize - 0.5f) / imageWidth, (y + ySize - 0.5f)  / imageHeight };

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
        int glyphsPerLine = imageGlyph.image.width / pxSize;

        int wSlot = imageGlyph.idx % glyphsPerLine;
        int hSlot = imageGlyph.idx / glyphsPerLine;

        get2DSquareBox(imageGlyph.offsetX, imageGlyph.offsetY, 1, vtxs);
        getUVs(uvs, wSlot * pxSize, hSlot * pxSize, pxSize, pxSize,
               imageGlyph.image.width, imageGlyph.image.height);
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
        hb_codepoint_t codepoint = text.getGlyph(n);

        //    printf("offsetX %d, offsetY: %d\n", text.getOffsetX(n),
        //    text.getOffsetY(n));

        auto gl = std::find(glyphCache.begin(), glyphCache.end(), codepoint);

        if (gl == glyphCache.end()) {
            int glyphsPerLine = image.image.width / face.pxSize;

            int wslot = image.idx % glyphsPerLine;
            int hslot = image.idx / glyphsPerLine;

            //      printf("New Glyph on slot %d %d, idx %d\n", wslot, hslot,
            //      image.idx);

            FT_Error error =
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

                float descender =
                    (float)(metrics.height - metrics.horiBearingY) / 64.0 /
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

        penX += ((float)text.getAdvanceX(n)) / 64.0 / (float)face.pxSize;
        penY += ((float)text.getAdvanceY(n)) / 64.0 / (float)face.pxSize;
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
