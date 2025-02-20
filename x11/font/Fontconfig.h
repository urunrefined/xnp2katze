#pragma once

#include <string>
#include <vector>

#include <fontconfig.h>

namespace BR {

class FontconfigLib {
  public:
    FcConfig *config = 0;

    FontconfigLib();
    ~FontconfigLib();
};

class FontList {
    FcFontSet *fs = 0;

  public:
    FontList(FontconfigLib &FontconfigLib);
    ~FontList();

    void print();
    std::string getFirst(const std::vector<const char *> &langs);
    std::vector<std::string>
    getFontList(const std::vector<const char *> &langs);
};

} // namespace BR
