#include "Harfbuzz.h"

#include <hb-ft.h>
#include <stdio.h>

namespace BR {

HarfbuzzText::HarfbuzzText(const char *text, HarfbuzzFont &font) {

    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    /*
      hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
      hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
      hb_buffer_set_language(buf, hb_language_from_string("en", -1));
      */

    hb_buffer_guess_segment_properties(buf);
    hb_shape(font.font, buf, NULL, 0);

    glyph_info = hb_buffer_get_glyph_infos(buf, 0);
    glyph_pos = hb_buffer_get_glyph_positions(buf, 0);

    glyph_count = hb_buffer_get_length(buf);
}

HarfbuzzText::HarfbuzzText(const std::vector<uint32_t> &codePoints,
                           HarfbuzzFont &font) {
    buf = hb_buffer_create();
    hb_buffer_add_utf32(buf, (uint32_t *)codePoints.data(),
                        (int)codePoints.size(), 0, -1);

    /*
      hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
      hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
      hb_buffer_set_language(buf, hb_language_from_string("en", -1));*/

    hb_buffer_guess_segment_properties(buf);
    hb_shape(font.font, buf, NULL, 0);

    glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
}

void HarfbuzzText::print() {

    for (unsigned int i = 0; i < glyph_count; i++) {
        hb_position_t x_offset = glyph_pos[i].x_offset;
        hb_position_t y_offset = glyph_pos[i].y_offset;
        hb_position_t x_advance = glyph_pos[i].x_advance;
        hb_position_t y_advance = glyph_pos[i].y_advance;

        //    hb_font_extents_t temp;

        printf(
            "i %u | gid: %u, x_offset %u, y_offset %u, x_advance %u, y_advance "
            "%u\n",
            i, glyph_info[i].codepoint, x_offset, y_offset, x_advance,
            y_advance);
    }
}

unsigned int HarfbuzzText::getGlyphCount() const { return glyph_count; }

hb_codepoint_t HarfbuzzText::getGlyph(unsigned int i) {
    return glyph_info[i].codepoint;
}

hb_position_t HarfbuzzText::getAdvanceX(unsigned int i) {
    return glyph_pos[i].x_advance;
}

hb_position_t HarfbuzzText::getAdvanceY(unsigned int i) {
    return glyph_pos[i].y_advance;
}

hb_position_t HarfbuzzText::getOffsetX(unsigned int i) {
    return glyph_pos[i].x_offset;
}

hb_position_t HarfbuzzText::getOffsetY(unsigned int i) {
    return glyph_pos[i].y_offset;
}

HarfbuzzText::~HarfbuzzText() { hb_buffer_destroy(buf); }

HarfbuzzBlob::HarfbuzzBlob(const char *fontfile) {
    blob = hb_blob_create_from_file(fontfile);

    if (!blob) {
        throw "No blob";
    }
}

HarfbuzzBlob::~HarfbuzzBlob() { hb_blob_destroy(blob); }

HarfbuzzFont::HarfbuzzFont(FT_Face ft_face, unsigned int fontSize) {
    font = hb_ft_font_create(ft_face, 0);

    if (!font) {
        throw "No font";
    }

    (void)fontSize;
}

HarfbuzzFont::~HarfbuzzFont() { hb_font_destroy(font); }

} // namespace BR
