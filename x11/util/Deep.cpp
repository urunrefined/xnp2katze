#include "Deep.h"
#include "Vertex.h"

#include <algorithm>

#include <string.h>

namespace BR {

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
    memset(image.data, color, image.width * image.height);
}

void fill(const Image8 &image) { fill(image, 0); }

static size_t px4(uint16_t imageWidth, uint16_t x, uint16_t y) {
    return (y * imageWidth + x) * 4;
}

void fill(const ImageBGRA &image) {
    auto data = image.data;

    memset(data, 0, image.width * image.height * 4);
}

void fill(const ImageBGRA &image, const Rect &rect, const ColorU8BGRA &color) {
    if (rect.offsetX >= image.width)
        return;
    if (rect.offsetY >= image.height)
        return;

    uint16_t xToEdge = image.width - rect.offsetX;
    uint16_t yToEdge = image.height - rect.offsetY;

    uint16_t xEnd = rect.offsetX + std::min(xToEdge, rect.sizeX);
    uint16_t yEnd = rect.offsetY + std::min(yToEdge, rect.sizeY);

    auto data = image.data;

    for (auto y = rect.offsetY; y < yEnd; y++) {
        for (auto x = rect.offsetX; x < xEnd; x++) {

            size_t offset = px4(image.width, x, y);

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
        size_t offset = px4(image.width, x, y);

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
        size_t offset = px4(image.width, x, y);

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

  Vec2 bl {begin.x       , begin.y        };
  Vec2 ul {begin.x       , begin.y + sz.y };
  Vec2 ur {begin.x + sz.x, begin.y + sz.y };
  Vec2 br {begin.x + sz.x, begin.y        };

    // clang-format on

    ret[0] = bl;
    ret[1] = ur;
    ret[2] = ul;

    ret[3] = bl;
    ret[4] = br;
    ret[5] = ur;
}

} // namespace BR
