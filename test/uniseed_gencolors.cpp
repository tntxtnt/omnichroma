#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "generate.h"
#include "color.h"
#include <random>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <vector>

TEST_CASE("Testing generate shuffled colors function") {
    unsigned int seed = 123;
    std::mt19937 urbg{seed};
    const auto colors = generateShuffledColors<Color>(4, urbg);
    REQUIRE(colors.size() == 4 * 4 * 4);
    const std::vector<Color> expectedColors{
        Color{0x000055}, Color{0xaa55ff}, Color{0x0055ff}, Color{0x000000}, Color{0x55ff55}, Color{0x0000ff},
        Color{0x555555}, Color{0x00ff00}, Color{0xaa55aa}, Color{0xff0000}, Color{0xaaffff}, Color{0xaa0055},
        Color{0x00aa00}, Color{0x005500}, Color{0x550000}, Color{0x00aaff}, Color{0xff5555}, Color{0xaa0000},
        Color{0xaaaaaa}, Color{0xff55aa}, Color{0x55aa00}, Color{0xffffaa}, Color{0xaaffaa}, Color{0xaaaaff},
        Color{0x00aaaa}, Color{0xff5500}, Color{0x5500ff}, Color{0xaa00ff}, Color{0xaa5555}, Color{0xffaa55},
        Color{0xff55ff}, Color{0x005555}, Color{0xff0055}, Color{0xaa5500}, Color{0xff00ff}, Color{0x00ffaa},
        Color{0x55aa55}, Color{0x0000aa}, Color{0x00aa55}, Color{0xffff55}, Color{0xffaaff}, Color{0x55ffaa},
        Color{0xffff00}, Color{0x00ffff}, Color{0xaaff55}, Color{0x5500aa}, Color{0x0055aa}, Color{0x5555ff},
        Color{0x55ff00}, Color{0xaa00aa}, Color{0x555500}, Color{0xff00aa}, Color{0xaaaa55}, Color{0x5555aa},
        Color{0x55aaff}, Color{0xffaa00}, Color{0xffaaaa}, Color{0x55ffff}, Color{0xaaaa00}, Color{0x00ff55},
        Color{0x55aaaa}, Color{0x550055}, Color{0xffffff}, Color{0xaaff00}};
    for (auto color : colors) fmt::print("{} ", color);
    std::cout << std::endl;
    CHECK(colors == expectedColors);
}