#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <optional>
#include <string>

#include "Codepage.h"

namespace BR {

static bool validContByte(unsigned char ch) {
    (void)ch;
    // todo:
    return true;
}

int nextUTF8(const unsigned char *chstr) {
    if (chstr[0] == '\0')
        return 0;

    // Single-byte
    if (!(chstr[0] & 0b1000'0000)) {
        return 1;
    }

    // multi-byte
    if ((chstr[0] & 0b1100'0000) == 0b1100'0000) {
        if (!(chstr[0] & 0b0010'0000)) {
            // Check extent
            if (validContByte(chstr[1])) {
                return 2;
            }
        }
    }

    if ((chstr[0] & 0b1110'0000) == 0b1110'0000) {
        if (!(chstr[0] & 0b0001'0000)) {
            if (validContByte(chstr[1]) && validContByte(chstr[2])) {
                return 3;
            }
        }
    }

    if ((chstr[0] & 0b1111'0000) == 0b1111'0000) {
        if (!(chstr[0] & 0b0000'1000)) {
            if (validContByte(chstr[1]) && validContByte(chstr[2]) &&
                validContByte(chstr[3])) {
                return 4;
            }
        }
    }

    return -1;
}

std::optional<std::vector<uint32_t>> getUnicodeFromUTF8(const char *utf8) {
    std::vector<uint32_t> codePoints;

    while (true) {
        int ret = nextUTF8((const unsigned char *)utf8);

        if (ret == 0)
            break;

        if (ret == -1) {
            printf("Error parsing UTF8 string\n");
            return std::nullopt;
        }

        uint32_t codePoint = 0;

        if (ret == 1) {
            codePoint = utf8[0];
        } else if (ret == 2) {
            codePoint |= (utf8[0] & 0b0001'1111) << 6;
            codePoint |= (utf8[1] & 0b0011'1111);
        } else if (ret == 3) {
            codePoint |= (utf8[0] & 0b0000'1111) << 12;
            codePoint |= (utf8[1] & 0b0011'1111) << 6;
            codePoint |= (utf8[2] & 0b0011'1111);
        } else if (ret == 4) {
            codePoint |= (utf8[0] & 0b0000'0111) << 18;
            codePoint |= (utf8[1] & 0b0011'1111) << 12;
            codePoint |= (utf8[2] & 0b0011'1111) << 6;
            codePoint |= (utf8[3] & 0b0011'1111);
        }

        codePoints.push_back(codePoint);

        utf8 += ret;
    }

    return codePoints;
}

std::optional<std::string>
getUTF8FromUnicode(const std::vector<uint32_t> &codePoints) {
    std::string str;

    for (auto e : codePoints) {
        if (e < 0x80) {
            str += (char)e;
        } else if (e < 0x800) {
            unsigned char utf8[2];

            utf8[0] = 0b1100'0000 | ((e & 0b111'1100'0000) >> 6);
            utf8[1] = 0b1000'0000 | ((e & 0b11'1111));

            str.append((const char *)utf8, 2);
        } else if (e < 0x10000) {
            char utf8[3];

            utf8[0] = 0b1110'0000 | ((e & 0b1111'0000'0000'0000) >> 12);
            utf8[1] = 0b1000'0000 | ((e & 0b0000'1111'1100'0000) >> 6);
            utf8[2] = 0b1000'0000 | ((e & 0b0000'0000'0011'1111));

            str.append((const char *)utf8, 3);

        } else if (e <= 0x10FFFF) {
            char utf8[4];

            utf8[0] =
                0b1111'0000 | ((e & 0b0001'1100'0000'0000'0000'0000) >> 18);
            utf8[1] =
                0b1000'0000 | ((e & 0b0000'0011'1111'0000'0000'0000) >> 12);
            utf8[2] =
                0b1000'0000 | ((e & 0b0000'0000'0000'1111'1100'0000) >> 6);
            utf8[3] = 0b1000'0000 | ((e & 0b0000'0000'0000'0000'0011'1111));

            str.append((const char *)utf8, 4);
        } else {
            printf("Invalid unicode code point %u\n", e);
            return std::nullopt;
        }
    }

    return str;
}

} // namespace BR
