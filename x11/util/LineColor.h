#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>

namespace BR {

enum ConsoleColor {
    WHITE = 0,
    BLUE = 1,
    RED = 2,
};

struct FormatString {
    const char *text;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatPad {
    size_t padLeft;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatInt {
    int i;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatUInt {
    unsigned int i;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatSize {
    size_t i;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatHex {
    uint8_t i;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatHex16 {
    uint16_t i;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatHex32 {
    uint32_t i;
    ConsoleColor color = ConsoleColor::WHITE;
};

struct FormatPrintChar {
    uint8_t ch;
    ConsoleColor color = ConsoleColor::WHITE;
};

template <size_t N> struct LineColor {
    char text[N]{};
    uint8_t colors[N]{};

    size_t cur = 0;

    LineColor &operator<<(const FormatString &formatString) {
        const size_t rem = N - cur;
        const size_t copy = std::min(rem, strlen(formatString.text));

        for (size_t i = 0; i < copy; i++) {
            text[cur] = formatString.text[i];
            colors[cur] = formatString.color;

            cur++;
        }

        return *this;
    }

    LineColor &operator<<(const FormatPad &formatPad) {
        if (formatPad.padLeft <= cur)
            return *this;

        size_t copy = formatPad.padLeft - cur;

        const size_t rem = N - cur;

        copy = std::min(rem, copy);

        for (size_t i = 0; i < copy; i++) {
            text[cur] = ' ';
            colors[cur] = formatPad.color;
            cur++;
        }

        return *this;
    }

    LineColor &operator<<(const FormatInt &formatInt) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%d", formatInt.i);

        return (*this) << FormatString{buf, formatInt.color};
    }

    LineColor &operator<<(const FormatUInt &formatUInt) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%u", formatUInt.i);

        return (*this) << FormatString{buf, formatUInt.color};
    }

    LineColor &operator<<(const FormatSize &formatSize) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%zu", formatSize.i);

        return (*this) << FormatString{buf, formatSize.color};
    }

    LineColor &operator<<(const FormatHex &formatHex) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%.2hhx", formatHex.i);

        return (*this) << FormatString{buf, formatHex.color};
    }

    LineColor &operator<<(const FormatHex16 &formatHex) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%.4hx", formatHex.i);

        return (*this) << FormatString{buf, formatHex.color};
    }

    LineColor &operator<<(const FormatHex32 &formatHex) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%.8x", formatHex.i);

        return (*this) << FormatString{buf, formatHex.color};
    }

    LineColor &operator<<(const FormatPrintChar &formatPrintChar) {
        char buf[128];

        snprintf(buf, sizeof(buf), "%c", formatPrintChar.ch);

        return (*this) << FormatString{buf, formatPrintChar.color};
    }

    void print() {
        printf("text [");

        for (auto x : text) {
            printf("%c", x);
        }

        printf("]\n");
    }

    void printColors() {
        printf("colors [");

        for (auto x : colors) {
            printf("%d,", x);
        }

        printf("]\n");
    }
};

} // namespace BR