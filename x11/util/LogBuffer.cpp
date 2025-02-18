#include "LogBuffer.h"

#include <string.h>

namespace BR {
size_t LogBuffer::add(const char *ln) {
    size_t next = (cur + 1) % ((sizeof(lines) / sizeof(lines[0])));

    LineColor<80> newLine;
    newLine << FormatString{ln, 0};

    newLine.print();

    lines[next] = newLine;
    cur = next;

    return cur;
}

size_t LogBuffer::add(const LineColor<80> &lineColor) {
    size_t next = (cur + 1) % ((sizeof(lines) / sizeof(lines[0])));

    lines[next] = lineColor;
    cur = next;

    return cur;
}

LineColor<80> *LogBuffer::get(size_t which) {
    which %= arraySize(lines);

    if (which > cur) {
        return &lines[arraySize(lines) - (which - cur)];
    } else {
        return &lines[cur - which];
    }
}

void LogBuffer::addColor(size_t which, uint8_t (&nColors)[80]) {
    memcpy(lines[which].colors, nColors, sizeof(lines[which].colors));
}
} // namespace BR
