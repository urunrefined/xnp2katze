#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Core.h"
#include "LineColor.h"

namespace BR {

class LogBuffer {
  public:
    LineColor<80> lines[1000]{};
    size_t cur = 0;

    size_t add(const char *ln);
    size_t add(const LineColor<80> &lineColor);
    LineColor<80> *get(size_t which);
    void addColor(size_t which, uint8_t (&nColors)[80]);
};

} // namespace BR
