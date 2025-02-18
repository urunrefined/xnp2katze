#pragma once

#include <stdint.h>

struct Rect {
    uint16_t offsetX;
    uint16_t offsetY;

    uint16_t sizeX;
    uint16_t sizeY;
};

struct Dimensions2D {
    uint32_t x;
    uint32_t y;

    bool operator==(const Dimensions2D &dims) const {
        return dims.x == x && dims.y == y;
    }

    bool operator!=(const Dimensions2D &dims) const { return !(*this == dims); }
};
