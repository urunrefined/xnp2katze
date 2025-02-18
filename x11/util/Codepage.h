#pragma once

#include <optional>
#include <stdint.h>
#include <string>
#include <vector>

namespace BR {

std::optional<std::vector<uint32_t>> getUnicodeFromUTF8(const char *utf8);

std::optional<std::string>
getUTF8FromUnicode(const std::vector<uint32_t> &codePoints);

} // namespace BR
