#pragma once

#include <stddef.h>
#include <stdint.h>

namespace BR {

struct ImageBGRA {
    uint16_t width;
    uint16_t height;
    char *data;

    size_t byteSize() const { return width * height * 4; }
};

struct Image8 {
    uint16_t width;
    uint16_t height;
    char *data;

    size_t byteSize() const { return width * height; }
};

} // namespace BR
