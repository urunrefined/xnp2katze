#include "Deep.h"
#include "Vertex.h"
#include "util/Image.h"
#include "util/Rect.h"
#include "util/Vec.h"

#include <algorithm>

#include <cstdint>
#include <string.h>
#include <vector>

namespace BR {

static bool lineHasData(const unsigned char *data, unsigned int width) {
    for (unsigned int x = 0; x < width * 4; x++) {
        if (data[x])
            return true;
    }

    return false;
}

struct LineSegment {
    uint16_t start;
    uint16_t size;
};

static std::vector<LineSegment>
getLineSegments(const unsigned char *data, uint16_t width, uint16_t pxCutOff) {
    uint16_t start = 0;
    uint16_t sz = 0;

    std::vector<LineSegment> blankSegments;

    for (uint16_t w = 0; w < width; w++) {
        if (sz == 0) {
            start = w;
        }

        if (data[w * 4 + 0] || data[w * 4 + 1] || data[w * 4 + 2] ||
            data[w * 4 + 3]) {
            if (sz >= pxCutOff) {
                blankSegments.push_back({start, sz});
            }

            sz = 0;
        } else {
            sz++;
        }
    }

    if (sz >= pxCutOff) {
        blankSegments.push_back({start, sz});
    }

    return blankSegments;
}

static uint16_t addAll(const std::vector<LineSegment> &segments) {
    uint16_t ret = 0;

    for (auto &segment : segments) {
        ret += segment.size;
    }

    return ret;
}

void doubleBlankLinesAdaptive(uint16_t width, uint16_t height,
                              const unsigned char *in, unsigned char *out,
                              uint16_t cutOff) {
    memcpy(out, in, (size_t)width * 4);

    for (uint16_t h = 1; h < height - 1; h++) {
        if (lineHasData(in + ((size_t)h + 1) * width * 4, width)) {
            memcpy(out + ((size_t)h) * width * 4, in + ((size_t)h) * width * 4,
                   (size_t)width * 4);
            auto lineSegments = getLineSegments(in + ((size_t)h) * width * 4,
                                                (size_t)width, 30);

            if (addAll(lineSegments) > cutOff) {
                for (auto &lineSegment : lineSegments) {

                    memcpy(out + ((size_t)h) * width * 4 +
                               ((size_t)lineSegment.start * 4),
                           in + ((size_t)h - 1) * width * 4 +
                               ((size_t)lineSegment.start * 4),
                           (size_t)lineSegment.size * 4);
                }
            }
        } else {
            memcpy(out + ((size_t)h) * width * 4, in + ((size_t)h) * width * 4,
                   (size_t)width * 4);
        }
    }

    memcpy(out + (((size_t)height - 1) * width * 4),
           in + (((size_t)height - 1) * width * 4), (size_t)width * 4);
}

void doubleBlankLines(uint16_t width, uint16_t height, const unsigned char *in,
                      unsigned char *out) {
    memcpy(out, in, (size_t)width * 4);

    for (uint16_t h = 1; h < height - 1; h++) {
        if (!lineHasData(in + (size_t)h * width * 4, width) &&
            lineHasData(in + ((size_t)h + 1) * width * 4, width)) {
            memcpy(out + ((size_t)h) * width * 4,
                   in + ((size_t)h - 1) * width * 4, (size_t)width * 4);
        } else {
            memcpy(out + ((size_t)h) * width * 4, in + ((size_t)h) * width * 4,
                   (size_t)width * 4);
        }
    }

    memcpy(out + (((size_t)height - 1) * width * 4),
           in + (((size_t)height - 1) * width * 4), (size_t)width * 4);
}

void fill(const ImageBGRA &image, const ColorU8BGRA &color) {
    auto data = image.data;

    for (uint32_t ray = 0; ray < image.width * image.height; ray++) {
        data[ray * 4 + 0] = color.b;
        data[ray * 4 + 1] = color.g;
        data[ray * 4 + 2] = color.r;
        data[ray * 4 + 3] = color.a;
    }
}

void fill(const Image8 &image, uint8_t color) {
    memset(image.data, color, (size_t)image.width * image.height);
}

void fill(const Image8 &image) { fill(image, 0); }

static size_t px4(uint16_t imageWidth, uint16_t x, uint16_t y) {
    return ((size_t)y * imageWidth + x) * 4;
}

void fill(const ImageBGRA &image) {
    auto data = image.data;

    memset(data, 0, (size_t)image.width * image.height * 4);
}

void fill(const ImageBGRA &image, const Rect &rect, const ColorU8BGRA &color) {
    if (rect.offsetX >= image.width)
        return;
    if (rect.offsetY >= image.height)
        return;

    const uint16_t xToEdge = image.width - rect.offsetX;
    const uint16_t yToEdge = image.height - rect.offsetY;

    const uint16_t xEnd = rect.offsetX + std::min(xToEdge, rect.sizeX);
    const uint16_t yEnd = rect.offsetY + std::min(yToEdge, rect.sizeY);

    auto data = image.data;

    for (auto y = rect.offsetY; y < yEnd; y++) {
        for (auto x = rect.offsetX; x < xEnd; x++) {

            const size_t offset = px4(image.width, x, y);

            data[offset + 0] = color.b;
            data[offset + 1] = color.g;
            data[offset + 2] = color.r;
            data[offset + 3] = color.a;
        }
    }
}

static void createHorizLine(const ImageBGRA &image, uint16_t y,
                            const ColorU8BGRA &color) {
    auto data = image.data;

    for (auto x = 0; x < image.width; x++) {
        const size_t offset = px4(image.width, x, y);

        data[offset + 0] = color.b;
        data[offset + 1] = color.g;
        data[offset + 2] = color.r;
        data[offset + 3] = color.a;
    }
}

static void createVertLine(const ImageBGRA &image, uint16_t x,
                           const ColorU8BGRA &color) {
    auto data = image.data;

    for (auto y = 0; y < image.height; y++) {
        const size_t offset = px4(image.width, x, y);

        data[offset + 0] = color.b;
        data[offset + 1] = color.g;
        data[offset + 2] = color.r;
        data[offset + 3] = color.a;
    }
}

void createGrid(const ImageBGRA &image, uint16_t everyX, uint16_t everyY,
                const ColorU8BGRA &color) {
    for (uint32_t x = everyX; x < image.width; x += everyX) {
        createVertLine(image, x, color);
    }

    for (uint32_t y = everyY; y < image.height; y += everyY) {
        createHorizLine(image, y, color);
    }
}

void get2DRectCW(const Vec2 &begin, const Vec2 &sz, Vec2 (&ret)[6]) {
    // clang-format off

  Vec2 const bl {begin.x       , begin.y        };
  Vec2 const ul {begin.x       , begin.y + sz.y };
  Vec2 const ur {begin.x + sz.x, begin.y + sz.y };
  Vec2 const br {begin.x + sz.x, begin.y        };

    // clang-format on

    ret[0] = bl;
    ret[1] = ur;
    ret[2] = ul;

    ret[3] = bl;
    ret[4] = br;
    ret[5] = ur;
}

} // namespace BR
