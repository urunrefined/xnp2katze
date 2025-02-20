#pragma once

#include <hb-ft.h>
#include <hb.h>
#include <vector>

namespace BR {

class HarfbuzzBlob {
  public:
    hb_blob_t *blob;

    HarfbuzzBlob(const char *fontfile);

    ~HarfbuzzBlob();
};

class HarfbuzzFont {
  public:
    hb_font_t *font;

    HarfbuzzFont(FT_Face ft_face, unsigned int fontSize);

    ~HarfbuzzFont();
};

class HarfbuzzText {
    hb_buffer_t *buf = 0;
    unsigned int glyph_count = 0;
    hb_glyph_info_t *glyph_info = 0;
    hb_glyph_position_t *glyph_pos = 0;

  public:
    HarfbuzzText(const char *text, HarfbuzzFont &font);
    HarfbuzzText(const std::vector<uint32_t> &codepoints, HarfbuzzFont &font);
    ~HarfbuzzText();

    void print();

    unsigned int getGlyphCount() const;

    hb_codepoint_t getGlyph(unsigned int i);
    hb_position_t getAdvanceX(unsigned int i);
    hb_position_t getAdvanceY(unsigned int i);

    hb_position_t getOffsetX(unsigned int i);
    hb_position_t getOffsetY(unsigned int i);
};

} // namespace BR
