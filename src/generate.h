#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <boost/random.hpp>

// Copy on cppref
template <class RandomIt, class URBG>
void cppref_shuffle(RandomIt first, RandomIt last, URBG&& g) {
    using diff_t = typename std::iterator_traits<RandomIt>::difference_type;
    using distr_t = boost::random::uniform_int_distribution<diff_t>;
    using param_t = typename distr_t::param_type;

    distr_t D;
    diff_t n = last - first;
    for (diff_t i = n - 1; i > 0; --i) {
        using std::swap;
        swap(first[i], first[D(g, param_t(0, i))]);
    }
}

template <class Func>
void forEachEquidistantChannelValue(int colorsPerChannel, Func&& yield) {
    for (uint32_t i = 0; i < colorsPerChannel; ++i) yield(static_cast<uint8_t>(i * 255 / (colorsPerChannel - 1)));
}

template <class Color, class URBG>
auto generateShuffledColors(int colorsPerChannel, URBG&& urbg) -> std::vector<Color> {
    std::vector<Color> colors;
    colors.reserve(colorsPerChannel * colorsPerChannel * colorsPerChannel);
    forEachEquidistantChannelValue(colorsPerChannel, [&](auto r) {
        forEachEquidistantChannelValue(colorsPerChannel, [&](auto g) {
            forEachEquidistantChannelValue(colorsPerChannel, [&](auto b) { colors.emplace_back(r, g, b); });
        });
    });
    cppref_shuffle(begin(colors), end(colors), urbg);
    return colors;
}
