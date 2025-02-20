#pragma once

#include "vk/VKBuffers.h"

namespace BR {

class GLImage {
  public:
    Segment2f vtxs;
    Segment2f uvs;
    uint32_t drawCount = 6;

    GLImage(DataAllocator &dataAllocator)
        : vtxs(dataAllocator.get2fSegment(6)),
          uvs(dataAllocator.get2fSegment(6)) {

        Vec2 vtxsData[6];
        get2DRectCW({0, 0}, {1.f, 1.f}, vtxsData);

        Vec2 uvsData[6];
        get2DRectCW({0, 0}, {1, 1}, uvsData);

        vtxs.update(ArrayView<Vec2>(vtxsData, 6));
        uvs.update(ArrayView<Vec2>(uvsData, 6));
    }
};

} // namespace BR
