#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Core.h"
#include "LineColor.h"

namespace BR {

class LogBuffer {
  public:
    LineColor<132> lines[1000]{};
    size_t cur = 0;

    size_t add(const char *ln);
    size_t add(const LineColor<132> &lineColor);
    LineColor<132> *get(size_t which);
    void addColor(size_t which, uint8_t (&nColors)[132]);
};

} // namespace BR
