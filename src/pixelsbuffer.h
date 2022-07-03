#pragma once

#include "color.h"
#include <optional>
#include <memory>
#include <cstdint>

struct Bitmap24bitOptionalPixels {
    using value_type = std::optional<Color>;

    uint32_t width;
    uint32_t height;
    std::unique_ptr<value_type[]> data;

    Bitmap24bitOptionalPixels(uint32_t width, uint32_t height);

    auto operator[](int i) const -> const value_type*;
    auto operator[](int i) -> value_type*;
    void reset();
};