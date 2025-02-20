#pragma once

#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "util/Image.h"
#include "util/Rect.h"
#include "util/Vertex.h"

#include "Freetype.h"
#include "Harfbuzz.h"

namespace BR {

class TextDims {
  public:
    long startX = 0, startY = 0;
    long sizeX = 0, sizeY = 0;

    TextDims(long startX_, long startY_, long sizeX_, long sizeY_)
        : startX(startX_), startY(startY_), sizeX(sizeX_), sizeY(sizeY_) {}

    TextDims() {}

    TextDims operator+=(const TextDims &dims) {
        long minX = std::min(startX, dims.startX);
        long minY = std::min(startY, dims.startY);

        long maxX = std::max(startX + sizeX, dims.startX + dims.sizeX);
        long maxY = std::max(startY + sizeY, dims.startY + dims.sizeY);

        startX = minX;
        startX = minY;
        sizeX = maxX - minX;
        sizeY = maxY - minY;

        return *this;
    }

    ~TextDims() {}
};

struct Mapping {
    hb_codepoint_t codepoint;
    uint32_t idx;
    float descender;
    Image8 image;

    bool operator==(hb_codepoint_t codepoint_) const {
        return codepoint == codepoint_;
    }
};

struct ImageGlyph {
    float offsetX;
    float offsetY;
    uint32_t idx;
    Image8 image;
};

struct ImageIndexed8 {
    Image8 image;
    unsigned int idx;
    bool dirty;
};

std::vector<ImageGlyph> getTextMappings(ImageIndexed8 &image,
                                        std::vector<Mapping> &textGlyphMapping,
                                        FreetypeFace &face, HarfbuzzText &text);

void mappingsToGlData(unsigned int px, std::vector<ImageGlyph> &imageGlyphs,
                      std::vector<Vec2> &vtxs, std::vector<Vec2> &uvs);

class Font {
  public:
    FreetypeLib freetypeLib;
    FreetypeFace freetypeFace;

    HarfbuzzBlob hbblob;
    HarfbuzzFont hbfont;

    Font(const char *fontfile, uint32_t fontSize)
        : freetypeFace(freetypeLib, fontfile, fontSize), hbblob(fontfile),
          hbfont(freetypeFace.face, fontSize) {}
};

// TODO: Fix issue with font sizes != 64
// TODO: Use R channel for alpha instead of rgba tex

std::string getFont();

} // namespace BR
