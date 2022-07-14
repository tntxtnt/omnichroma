#pragma once

#include "coord.h"
#include <string>
#include <unordered_set>
#include <boost/unordered/unordered_set.hpp>
#include <fmt/format.h>

template <class CoordSetType>
struct CoordSetWrapper {
    using type = CoordSetType;
    static auto name() -> std::string {
        return fmt::format("unk{}", boost::hash<std::string>{}(typeid(CoordSetType).name()));
    }
};


struct CoordStdUnorderedSet {
    using type = std::unordered_set<Coord>;
    static auto name() -> std::string { return "sus"; }
};

struct CoordBoostUnorderedSet {
    using type = boost::unordered_set<Coord, std::hash<Coord>>;
    static auto name() -> std::string { return "bus"; }
};