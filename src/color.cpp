#include "color.h"
#include <iomanip>

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

auto operator<<(std::ostream& os, Color color) -> std::ostream& {
    const auto oldFill = os.fill();
    os.fill('0');
    os << '#' << std::hex << std::setw(2) << static_cast<int>(color.r) << std::setw(2) << static_cast<int>(color.g)
       << std::setw(2) << static_cast<int>(color.b) << std::dec;
    os.fill(oldFill);
    return os;
}

auto Color::operator==(Color other) const -> bool {
    return r == other.r && b == other.b && g == other.g;
}