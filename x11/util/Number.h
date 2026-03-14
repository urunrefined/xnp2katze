#pragma once

#include <stdlib.h>

namespace BR {

long atol16(const char *str) { return strtol(str, 0, 16); }
} // namespace BR