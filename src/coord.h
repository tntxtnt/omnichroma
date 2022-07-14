#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <array>
#include <bit>

struct Coord {
    uint32_t x = 0;
    uint32_t y = 0;
    auto operator==(const Coord& rhs) const -> bool;
};

auto fnv1aIntHash(int n) -> size_t;

template <>
struct std::hash<Coord> {
    auto operator()(const Coord& p) const noexcept -> size_t {
        size_t res = 0;
        auto hashCombine = [&](int v) { res ^= v + 0x9e3779b9ULL + (res << 6) + (res >> 2); };
        hashCombine(p.x);
        hashCombine(p.y);
        return res;
    }
};

auto operator<<(std::ostream& os, Coord coord) -> std::ostream&;
