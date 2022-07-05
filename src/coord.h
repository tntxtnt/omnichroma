#pragma once

#include <cstddef>
#include <functional>
#include <ostream>
#include <array>

struct Coord {
    int x = 0;
    int y = 0;
    auto operator==(const Coord& rhs) const -> bool;
};

static auto fnv1aIntHash(int n) -> size_t {
    auto doHash = [n](const size_t fnvOffsetBasis, const size_t fnvPrime) {
        size_t res = fnvOffsetBasis;
        for (unsigned char ch : std::bit_cast<std::array<unsigned char, sizeof(int)>>(n)) {
            res ^= ch;
            res *= fnvPrime;
        }
        return res;
    };
    if constexpr (sizeof(size_t) == 8) {
        constexpr size_t fnvOffsetBasis = 14695981039346656037ULL;
        constexpr size_t fnvPrime = 1099511628211ULL;
        return doHash(fnvOffsetBasis, fnvPrime);
    } else if constexpr (sizeof(size_t) == 4) {
        constexpr size_t fnvOffsetBasis = 2166136261U;
        constexpr size_t fnvPrime = 16777619U;
        return doHash(fnvOffsetBasis, fnvPrime);
    } else {
        static_assert("size_t's size must be either 4 or 8 bytes");
        return 0;
    }
}

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