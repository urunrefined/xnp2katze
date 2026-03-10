#pragma once

#include <stddef.h>
#include <stdint.h>

namespace BR {

struct ImageBGRA {
    uint16_t width;
    uint16_t height;
    uint8_t *data;

    size_t byteSize() const { return (size_t)width * height * 4; }
};

struct Image8 {
    uint16_t width;
    uint16_t height;
    uint8_t *data;

    size_t byteSize() const { return (size_t)width * height; }
};

} // namespace BR
