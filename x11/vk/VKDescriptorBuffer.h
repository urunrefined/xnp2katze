#pragma once

#include "VKBuffers.h"

#include <assert.h>

namespace BR {

class UniformChunk {
  public:
    VulkanUniformBuffer &buffer;
    const size_t bufferOffset;
    const size_t sz;

    void update(const char *data, size_t elementOffset, size_t dataSize) {
        assert(dataSize + elementOffset <= sz);
        buffer.update(data, elementOffset + bufferOffset, dataSize);
    }
};

} // namespace BR
