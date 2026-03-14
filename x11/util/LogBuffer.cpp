#include "LogBuffer.h"
#include "util/Core.h"
#include "util/LineColor.h"

#include <cstdint>
#include <string.h>

namespace BR {
size_t LogBuffer::add(const char *ln) {
    const size_t next = (cur + 1) % ((sizeof(lines) / sizeof(lines[0])));

    LineColor<132> newLine;
    newLine << FormatString{ln, 0};

    newLine.print();

    lines[next] = newLine;
    cur = next;

    return cur;
}

size_t LogBuffer::add(const LineColor<132> &lineColor) {
    const size_t next = (cur + 1) % ((sizeof(lines) / sizeof(lines[0])));

    lines[next] = lineColor;
    cur = next;

    return cur;
}

LineColor<132> *LogBuffer::get(size_t which) {
    which %= arraySize(lines);

    if (which > cur) {
        return &lines[arraySize(lines) - (which - cur)];
    } else {
        return &lines[cur - which];
    }
}

void LogBuffer::addColor(size_t which, uint8_t (&nColors)[132]) {
    memcpy(lines[which].colors, nColors, sizeof(lines[which].colors));
}
} // namespace BR
