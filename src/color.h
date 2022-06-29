#pragma once

#include <cstdint>

struct Color {
    uint8_t b = 255;
    uint8_t g = 255;
    uint8_t r = 255;

    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b) : r{r}, g{g}, b{b} {}
    explicit Color(uint32_t color)
    : b{static_cast<uint8_t>(color & 0xff)},
      g{static_cast<uint8_t>((color >> 8) & 0xff)},
      r{static_cast<uint8_t>((color >> 16) & 0xff)} {}
};

constexpr int sqDiff(Color c1, Color c2) {
    const int r = c1.r - c2.r;
    const int g = c1.g - c2.g;
    const int b = c1.b - c2.b;
    return r * r + g * g + b * b;
}