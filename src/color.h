#pragma once

#include <cstdint>

struct Color {
    uint8_t b = 255;
    uint8_t g = 255;
    uint8_t r = 255;

    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b);
    explicit Color(uint32_t color);
};

auto sqDiff(Color c1, Color c2) -> int;