#pragma once

#include <stdint.h>
#include <vector>

namespace BR {

class Image {
  public:
    const uint16_t width;
    const uint16_t height;

    std::vector<unsigned char> data;

    Image(uint16_t width_, uint16_t height_);
};

} // namespace BR

