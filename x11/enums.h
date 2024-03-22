#pragma once

#include <stdint.h>

enum class ViewPortMode : uint8_t { ASPECT = 0, STRETCH, INTEGER, END };

enum class VisualScreen : uint8_t { MAIN = 0, CFG, END };

enum class DoubleLines : uint8_t { NO = 0, SIMPLE, END };