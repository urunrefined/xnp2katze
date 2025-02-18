#pragma once

#include "Vertex.h"

namespace BR {

struct Palette8f {
    Vec3 colors[256]{};

    Palette8f(Vec3 defaultColor) {
        for (auto &color : colors) {
            color = defaultColor;
        }
    }
};

} // namespace BR
