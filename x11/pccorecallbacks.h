#pragma once

#include "vk/Input.h"
#include "vk/VKTexture.h"

namespace BR {

struct CallbackContext {
    VulkanTexture *texture;
    Input *input;
};

} // namespace BR
