#include "pixelsbuffer.h"

Bitmap24bitOptionalPixels::Bitmap24bitOptionalPixels(uint32_t width, uint32_t height)
: width{width}, height{height}, data{std::make_unique<value_type[]>(static_cast<size_t>(width) * height)} {}

auto Bitmap24bitOptionalPixels::operator[](int i) const -> const value_type* {
    return &data[i * width];
}

auto Bitmap24bitOptionalPixels::operator[](int i) -> value_type* {
    return &data[i * width];
}

void Bitmap24bitOptionalPixels::reset() {
    for (size_t i = 0, wh = static_cast<size_t>(width) * height; i < wh; ++i) data[i].reset();
}
