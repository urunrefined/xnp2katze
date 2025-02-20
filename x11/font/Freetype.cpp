#include "Freetype.h"

namespace BR {

FreetypeLib::FreetypeLib() {
    FT_Init_FreeType(&library);

    if (!library) {
        throw "Init freetype failed";
    }
}

FreetypeLib::~FreetypeLib() { FT_Done_FreeType(library); }

FreetypeFace::FreetypeFace(FreetypeLib &library, const char *fontfile,
                           FT_UInt ftpx)
    : pxSize(ftpx) {
    FT_New_Face(library.library, fontfile, 0, &face);
    if (!face) {
        throw "No face";
    }

    printf("pxSize-- %u\n", pxSize);
    FT_Set_Char_Size(face, pxSize * 64, pxSize * 64, 0, 0);

    descender = face->size->metrics.descender / pxSize + 1;
    lineheight = (face->size->metrics.height / pxSize) + 1;
    printf("lineheight: %u\n", lineheight);
    printf("descender: %d\n", descender);

    slot = face->glyph;
}

FreetypeFace::~FreetypeFace() { FT_Done_Face(face); }

} // namespace BR
