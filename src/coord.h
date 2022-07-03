#pragma once

#include <cstddef>
#include <functional>
#include <ostream>

struct Coord {
    int x = 0;
    int y = 0;
    auto operator==(const Coord& rhs) const -> bool;
};

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