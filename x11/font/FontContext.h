#pragma once

#include "FreeFont.h"

namespace BR {

struct FontContext {
    std::vector<Mapping> &textGlyphMappingCache;
    ImageIndexed8 &imageIndexed;
    FreetypeFace &freetypeFace;
    HarfbuzzFont &harfbuzzFont;
};

} // namespace BR