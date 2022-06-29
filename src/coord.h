#pragma once

#include <cstddef>
#include <functional>
#include <ostream>

template <class T>
void hash_combine(size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct Coord {
    int x = 0;
    int y = 0;
    bool operator==(const Coord& rhs) const { return x == rhs.x && y == rhs.y; }
};

template <>
struct std::hash<Coord> {
    size_t operator()(const Coord& p) const noexcept {
        size_t res = 0;
        hash_combine(res, p.x);
        hash_combine(res, p.y);
        return res;
    }
};

std::ostream& operator<<(std::ostream& os, Coord coord) {
    return os << '(' << coord.x << ", " << coord.y << ')';
}