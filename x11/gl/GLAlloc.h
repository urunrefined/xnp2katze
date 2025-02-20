#pragma once

#include "util/Array.h"
#include "util/Vertex.h"
#include "vk/VKBuffers.h"
#include "vk/VKDescriptorBuffer.h"

namespace BR {

template <class T1> class Segment {
  public:
    VulkanVtxBuffer *gBuffer;
    const size_t byteOffset;
    const size_t byteCount;
    const size_t elementCount;

    Segment(VulkanVtxBuffer *gBuffer_, size_t byteOffset_, size_t elementCount_)
        : gBuffer(gBuffer_), byteOffset(byteOffset_),
          byteCount(elementCount_ * sizeof(T1)), elementCount(elementCount_) {}

    void updatePartial(ArrayView<T1> buffer, size_t elementOffset) {
        if (!buffer.count)
            return;

        size_t updateByteOffset = elementOffset * sizeof(T1);

        gBuffer->update((char *)buffer.data(), byteOffset + updateByteOffset,
                        buffer.byteSize());
    }

    void update(ArrayView<T1> buffer) {
        assert(buffer.byteSize() <= byteCount);
        if (!buffer.count)
            return;

        gBuffer->update((char *)buffer.data(), byteOffset, buffer.byteSize());
    }

    size_t getElementSize() { return sizeof(T1); }
};

using Segment2f = Segment<Vec2>;
using Segment3f = Segment<Vec3>;
using Segment4f = Segment<Vec4>;

using Segment1u32 = Segment<uint32_t>;

class DataAllocator {
  public:
    VulkanVtxBuffer &buffer;
    size_t currentByte;

    DataAllocator(VulkanVtxBuffer &buffer_) : buffer(buffer_), currentByte(0) {}

    Segment2f get2fSegment(size_t elementCount) {
        size_t toAllocate = elementCount * 2 * sizeof(float);

        Segment2f segment{&buffer, currentByte, elementCount};

        currentByte += toAllocate;

        return segment;
    }

    Segment3f get3fSegment(size_t elementCount) {
        size_t toAllocate = elementCount * 3 * sizeof(float);

        Segment3f segment{&buffer, currentByte, elementCount};

        currentByte += toAllocate;

        return segment;
    }

    Segment4f get4fSegment(size_t elementCount) {
        size_t toAllocate = elementCount * 4 * sizeof(float);

        Segment4f segment{&buffer, currentByte, elementCount};

        currentByte += toAllocate;

        return segment;
    }

    template <class T1> Segment<T1> getSegment(ArrayView<T1> data) {
        size_t toAllocate = data.byteSize();

        Segment<T1> segment{&buffer, currentByte, data.count};

        segment.update(data);

        currentByte += toAllocate;

        return segment;
    }
};

class UniformAllocator {
  public:
    VulkanUniformBuffer &buffer;
    size_t currentByte;

    UniformAllocator(VulkanUniformBuffer &buffer_)
        : buffer(buffer_), currentByte(0) {}

    UniformChunk getChunk(size_t alignment, size_t size) {

        size_t nextStep = currentByte % alignment;

        currentByte += nextStep;

        UniformChunk chunk{buffer, currentByte, size};

        currentByte += size;

        return chunk;
    }
};

} // namespace BR
