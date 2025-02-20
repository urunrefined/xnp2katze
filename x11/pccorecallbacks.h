#pragma once

#include "glfw/Input.h"
#include "util/Image.h"

namespace BR {

struct CallbackContext {
    ImageBGRA image;
    Input *input;
    bool dirty;
};

} // namespace BR
