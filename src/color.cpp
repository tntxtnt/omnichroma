#include "color.h"

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r{r}, g{g}, b{b} {}

Color::Color(uint32_t color)
: b{static_cast<uint8_t>(color & 0xff)},
    g{static_cast<uint8_t>((color >> 8) & 0xff)},
    r{static_cast<uint8_t>((color >> 16) & 0xff)} {}

auto sqDiff(Color c1, Color c2) -> int {
    const int r = c1.r - c2.r;
    const int g = c1.g - c2.g;
    const int b = c1.b - c2.b;
    return r * r + g * g + b * b;
}