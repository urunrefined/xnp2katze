#include <stddef.h>
#include <stdio.h>
#include <string.h>

namespace BR {

struct StringView {
    const char *str;
    size_t sz;

    bool operator==(const StringView &v) {
        if (v.sz != sz) {
            return false;
        }

        return memcmp(v.str, str, sz) == 0;
    }

    bool operator==(const char *c) {
        if (strlen(c) != sz) {
            return false;
        }

        return memcmp(c, str, sz) == 0;
    }
};

struct StringView toStringView(const char *str);

struct Tokens {
    StringView views[16];
    size_t count = 0;
};

Tokens split(const char *data, char split = ' ');

} // namespace BR