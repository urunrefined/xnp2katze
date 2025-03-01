#include "StringView.h"
#include "Core.h"

namespace BR {

struct StringView toStringView(const char *str) { return {str, strlen(str)}; }

Tokens split(const char *data, char split) {
    Tokens tokens;

    for (size_t i = 0; i < arraySize(tokens.views); i++) {
        size_t sz = 0;

        while (1) {
            if (data[sz] == '\0') {
                if (sz) {
                    tokens.views[i] = {data, sz};
                    tokens.count++;
                }
                return tokens;
            }

            if (data[sz] == split) {
                if (sz == 0) {
                    data++;
                } else {
                    tokens.views[i] = {data, sz};
                    data += sz + 1;
                    tokens.count++;
                    break;
                }
            } else {
                sz++;
            }
        }
    }

    return tokens;
}

} // namespace BR