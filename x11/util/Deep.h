#pragma once

#include "Image.h"
#include "Rect.h"
#include "Vec.h"
#include "Vertex.h"

namespace BR {

void fill(const ImageBGRA &image);
void fill(const ImageBGRA &image, const ColorU8BGRA &color);
void fill(const ImageBGRA &image, const Rect &rect, const ColorU8BGRA &color);
void createGrid(const ImageBGRA &image, uint16_t everyX, uint16_t everyY,
                const ColorU8BGRA &color);

void fill(const Image8 &image);
void fill(const Image8 &image, uint8_t alpha);

void get2DRectCW(const Vec2 &begin, const Vec2 &sz, Vec2 (&ret)[6]);

} // namespace BR
