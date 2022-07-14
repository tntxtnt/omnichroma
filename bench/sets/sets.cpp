#include "omnichroma.h"
#include <BS_thread_pool.hpp>
#include <fmt/format.h>
#include <iostream>
#include <string>

#include <boost/unordered_set.hpp>
#include <tsl/hopscotch_set.h>
#include <tsl/ordered_set.h>
#include <tsl/sparse_set.h>
#include <tsl/robin_set.h>

template <class Func>
auto timeit(std::string_view name, int repeat, Func&& f) {
    fmt::print("{}: repeat {} time{}, total = ", name, repeat, repeat == 1 ? "" : "s");
    std::cout << std::flush;
    const auto startTime = std::chrono::steady_clock::now();
    for (int i = 0; i < repeat; ++i) f();
    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - startTime;
    fmt::print("{:.6f}s, avg = {:.6f}s", elapsed.count(), elapsed.count() / repeat);
    std::cout << std::endl;
}

template <class CoordSetType>
void benchImage256x128(std::string_view label, int repeat, unsigned int seed) {
    timeit(label, repeat, [&] {
        auto img = Omnichroma<CoordSetWrapper<CoordSetType>>::Image256x128();
        return img.generate(seed, 128, 64);
    });
}

template <class CoordSetType>
void benchImage512x512(std::string_view label, int repeat, unsigned int seed, BS::thread_pool& pool) {
    timeit(label, repeat, [&] {
        auto img = Omnichroma<CoordSetWrapper<CoordSetType>>::Image512x512();
        return img.generate(seed, 256, 256, &pool);
    });
}

auto main() -> int {
    constexpr unsigned int seed = 123;

    BS::thread_pool pool;
    fmt::print("Num threads = {}\n", pool.get_thread_count());

    fmt::print("Benching image 256x128...");
    std::cout << std::endl;
    int repeat = 100;
    benchImage256x128<std::unordered_set<Coord>>( //
        "  std::unordered_set", repeat, seed);
    benchImage256x128<boost::unordered_set<Coord, std::hash<Coord>>>( //
        "boost::unordered_set", repeat, seed);
    benchImage256x128<tsl::hopscotch_set<Coord>>( //
        "  tsl::hopscotch_set", repeat, seed);
    benchImage256x128<tsl::ordered_set<Coord>>( //
        "    tsl::ordered_set", repeat, seed);
    benchImage256x128<tsl::sparse_set<Coord>>( //
        "     tsl::sparse_set", repeat, seed);
    benchImage256x128<tsl::robin_set<Coord>>( //
        "      tsl::robin_set", repeat, seed);

    fmt::print("Benching image 512x512...");
    std::cout << std::endl;
    repeat = 10;
    benchImage512x512<std::unordered_set<Coord>>( //
        "  std::unordered_set", repeat, seed, pool);
    benchImage512x512<boost::unordered_set<Coord, std::hash<Coord>>>( //
        "boost::unordered_set", repeat, seed, pool);
    benchImage512x512<tsl::hopscotch_set<Coord>>( //
        "  tsl::hopscotch_set", repeat, seed, pool);
    benchImage512x512<tsl::ordered_set<Coord>>( //
        "    tsl::ordered_set", repeat, seed, pool);
    benchImage512x512<tsl::sparse_set<Coord>>( //
        "     tsl::sparse_set", repeat, seed, pool);
    benchImage512x512<tsl::robin_set<Coord>>( //
        "      tsl::robin_set", repeat, seed, pool);
}