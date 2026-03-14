
#include "Fontconfig.h"

#include <cstdio>
#include <fontconfig.h>
#include <string.h>
#include <string>
#include <vector>

namespace BR {

FontconfigLib::FontconfigLib() {
    config = FcInitLoadConfigAndFonts();
    if (!config) {
        throw "Loading Fontconfig failed";
    }

    FcConfigSetRescanInterval(config, 0);
}

FontconfigLib::~FontconfigLib() {
    FcConfigDestroy(config);
    FcFini();
}

class FontPattern {
  public:
    FcFontSet *font_patterns;

    FontPattern(FcConfig *config, FcPattern *pat, FcFontSet *fs) {
        FcResult result;

        font_patterns = FcFontSort(config, pat, FcTrue, 0, &result);

        if (!font_patterns || font_patterns->nfont == 0) {
            throw "No fonts installed on the system";
        }

        for (int j = 0; j < font_patterns->nfont; j++) {
            FcPattern *font_pattern;

            font_pattern =
                FcFontRenderPrepare(NULL, pat, font_patterns->fonts[j]);
            if (font_pattern) {
                FcFontSetAdd(fs, font_pattern);
            }
        }
    }

    ~FontPattern() { FcFontSetSortDestroy(font_patterns); }
};

class FontconfigPattern {
  public:
    FcPattern *pat;

    FontconfigPattern(FcConfig *config) {
        pat = FcNameParse((FcChar8 *)"monospace");
        FcConfigSubstitute(config, pat, FcMatchPattern);
        FcDefaultSubstitute(pat);

        if (!pat) {
            throw "Loading Fontconfig failed failed";
        }
    }

    ~FontconfigPattern() { FcPatternDestroy(pat); }
};

FontList::FontList(FontconfigLib &fontconfigLib) {
    const FontconfigPattern pat(fontconfigLib.config);

    fs = FcFontSetCreate();

    if (!fs) {
        throw "Obtaining fontlist failed";
    }

    const FontPattern fontPatterns(fontconfigLib.config, pat.pat, fs);
}

void FontList::print() {
    printf("Total fonts: %d\n", fs->nfont);
    for (int i = 0; i < fs->nfont; i++) {
        const FcPattern *font = fs->fonts[i];

        FcChar8 *file;
        FcPatternGetString(font, FC_FILE, 0, &file);

        printf("Font %d: [%s]\n", i, file);
    }
}

std::vector<std::string>
FontList::getFontList(const std::vector<const char *> &requestedLangs) {
    std::vector<std::string> ret;

//    print();

    for (int i = 0; i < fs->nfont; i++) {
        const FcPattern *font = fs->fonts[i];

        FcChar8 *style;
        FcPatternGetString(font, FC_STYLE, 0, &style);

        if (strcmp((const char *)style, "Regular") != 0) {
            continue;
        }

        FcBool scaleable;
        FcPatternGetBool(font, FC_SCALABLE, 0, &scaleable);

        if (!scaleable)
            continue;

        FcBool outline;
        FcPatternGetBool(font, FC_OUTLINE, 0, &outline);

        if (!outline)
            continue;

        FcLangSet *langSet;
        FcPatternGetLangSet(font, FC_LANG, 0, &langSet);

        bool langUnavailable = false;

        for (auto &requestedLang : requestedLangs) {
            if (FcLangSetHasLang(langSet, (FcChar8 *)requestedLang) !=
                FcLangEqual) {
                langUnavailable = true;
                break;
            };
        }

        if (langUnavailable)
            continue;

        FcChar8 *file;
        FcPatternGetString(font, FC_FILE, 0, &file);

        FcChar8 *format;
        FcPatternGetString(font, FC_FONTFORMAT, 0, &format);

        if (file) {
            ret.push_back((const char *)file);
        }
    }

    return ret;
}

std::string
FontList::getFirst(const std::vector<const char *> &requestedLangs) {
    auto fonts = getFontList(requestedLangs);

    if (!fonts.empty())
        return fonts[0];

    return "";
}

FontList::~FontList() { FcFontSetDestroy(fs); }

} // namespace BR
