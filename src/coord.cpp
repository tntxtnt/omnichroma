#include "coord.h"

auto Coord::operator==(const Coord& rhs) const -> bool { return x == rhs.x && y == rhs.y; }

auto operator<<(std::ostream& os, Coord coord) -> std::ostream& {
    return os << '(' << coord.x << ", " << coord.y << ')';
}