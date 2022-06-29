#pragma once

#include <cstddef>
#include <functional>
#include <ostream>

struct Coord {
    int x = 0;
    int y = 0;
    auto operator==(const Coord& rhs) const -> bool;
};

template <class T>
void hash_combine(size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <>
struct std::hash<Coord> {
    auto operator()(const Coord& p) const noexcept -> size_t {
        size_t res = 0;
        hash_combine(res, p.x);
        hash_combine(res, p.y);
        return res;
    }
};

auto operator<<(std::ostream& os, Coord coord) -> std::ostream&;