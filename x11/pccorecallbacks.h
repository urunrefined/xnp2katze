#pragma once

#include "glfw/Input.h"
#include "vk/VKTexture.h"

namespace BR {

struct CallbackContext {
    VulkanTexture *texture;
    Input *input;
};

} // namespace BR
