#include "coord.h"

auto Coord::operator==(const Coord& rhs) const -> bool { return x == rhs.x && y == rhs.y; }

auto operator<<(std::ostream& os, Coord coord) -> std::ostream& {
    return os << '(' << coord.x << ", " << coord.y << ')';
}

auto fnv1aIntHash(int n) -> size_t {
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
