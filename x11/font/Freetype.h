#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdint.h>

namespace BR {

class FreetypeLib {
  public:
    FT_Library library = 0;

    FreetypeLib();
    ~FreetypeLib();
};

class FreetypeFace {
  public:
    FT_Face face = 0;
    FT_UInt pxSize = 0;

    //    uint32_t descender;
    uint32_t lineheight;
    FT_GlyphSlot slot;

    FreetypeFace(FreetypeLib &library, const char *fontfile, FT_UInt ftpx);
    ~FreetypeFace();
};

} // namespace BR
