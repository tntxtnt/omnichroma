#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <vector>
#include <boost/unordered_set.hpp>
#include "omnichroma.h"
#include <BS_thread_pool.hpp>

TEST_CASE("Testing omnichroma") {
    unsigned int seed = 123;
    std::hash<int> hasher;
    std::cout << hasher(0) << "\n";
    std::cout << hasher(1) << "\n";
    std::cout << hasher(2) << "\n";
    BS::thread_pool pool;
    fmt::print("Num threads = {}\n", pool.get_thread_count());
    // using omnichroma_t = Omnichroma<boost::unordered_set<Coord, std::hash<Coord>>>;
    // using omnichroma_t = Omnichroma<tsl::hopscotch_set<Coord>>;
    using omnichroma_t = Omnichroma<>;

    auto img = omnichroma_t::Image256x128();
    img.generate(seed, 128, 64);
    img.savePng();
    img.generate(seed, 128, 64, &pool);
    img.savePng();
}