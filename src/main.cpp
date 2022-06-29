#include "omnichroma.h"
#include <fmt/format.h>
#include <BS_thread_pool.hpp>
#include <chrono>

template <class Func, class... Args>
void timeit(int repeat, Func&& f, Args&&... args) {
    using namespace std::chrono;
    const auto startTime = steady_clock::now();
    for (int i = 0; i < repeat; ++i) f(std::forward<Args>(args)...);
    duration<double> elapsed = steady_clock::now() - startTime;
    fmt::print("Run {} time{}, total = {}s, avg = {}s\n", repeat, repeat == 1 ? "" : "s", elapsed.count(),
               elapsed.count() / repeat);
}

auto main(int argc, char** argv) -> int {
    const auto seed = std::random_device{}();
    fmt::print("Seed = {}\n", seed);

    BS::thread_pool pool;
    fmt::print("Num threads = {}\n", pool.get_thread_count());

    auto gen = [](Omnichroma& img, uint32_t seed, BS::thread_pool* pPool) {
        img.generate(seed, pPool);
        img.save();
    };

    /*{
        auto img = Omnichroma::Image32x16();
        timeit(1, gen, img, seed, nullptr);
        timeit(1, gen, img, seed, &pool);
    }
    {
        auto img = Omnichroma::Image64x64();
        timeit(1, gen, img, seed, nullptr);
        timeit(1, gen, img, seed, &pool);
    }
    {
        auto img = Omnichroma::Image256x128();
        timeit(1, gen, img, seed, nullptr);
        timeit(1, gen, img, seed, &pool);
    }*/
    {
        auto img = Omnichroma::Image512x512();
        timeit(1, gen, img, seed, nullptr);
        //timeit(1, gen, img, seed, &pool);
    }
}