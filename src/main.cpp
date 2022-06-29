#include "omnichroma.h"
#include <fmt/format.h>
#include <BS_thread_pool.hpp>
#include <chrono>
#include <string>
#include <random>

template <class Func>
auto timeit(std::string_view name, int repeat, Func&& f) {
    const auto startTime = std::chrono::steady_clock::now();
    for (int i = 0; i < repeat; ++i) f();
    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - startTime;
    fmt::print("{}: repeat {} time{}, total = {}s, avg = {}s\n", name, repeat, repeat == 1 ? "" : "s", elapsed.count(),
               elapsed.count() / repeat);
}

auto main(int argc, char** argv) -> int {
    const auto seed = std ::random_device{}();
    fmt::print("Seed = {}\n", seed);

    BS::thread_pool pool;
    fmt::print("Num threads = {}\n", pool.get_thread_count());

    auto gen = [](Omnichroma& img, uint32_t seed, int startX, int startY, BS::thread_pool* pPool) {
        img.generate(seed, startX, startY, pPool);
    };

    {
        auto img = Omnichroma::Image32x16();
        timeit("32x16 single thread", 100, [&] { return img.generate(seed, 16, 8); });
        img.savePng();
        timeit("32x16 threadpool   ", 100, [&] { return img.generate(seed, 16, 8, &pool); });
        img.savePng();
    }
    {
        auto img = Omnichroma::Image64x64();
        timeit("64x64 single thread", 10, [&] { return img.generate(seed, 32, 32); });
        img.savePng();
        timeit("64x64 threadpool   ", 10, [&] { return img.generate(seed, 32, 32, &pool); });
        img.savePng();
    }
    {
        auto img = Omnichroma::Image256x128();
        timeit("256x128 single thread", 1, [&] { return img.generate(seed, 128, 64); });
        img.savePng();
        timeit("256x128 threadpool   ", 1, [&] { return img.generate(seed, 128, 64, &pool); });
        img.savePng();
    }
    {
        auto img = Omnichroma::Image512x512();
        timeit("512x512 single thread", 1, [&] { return img.generate(seed, 256, 256); });
        timeit("512x512 threadpool   ", 1, [&] { return img.generate(seed, 256, 256, &pool); });
        img.savePng();
    }
    {
        auto img = Omnichroma::Image2048x1024();
        timeit("2048x1024 single thread", 1, [&] { return img.generate(seed, 1024, 512); });
        timeit("2048x1024 threadpool   ", 1, [&] { return img.generate(seed, 1024, 512, &pool); });
        img.savePng();
    }
    {
        auto img = Omnichroma::Image4096x4096();
        // timeit("4096x4096 single thread", 1, [&] { return img.generate(seed, 2048, 2048); });
        timeit("4096x4096 threadpool   ", 1, [&] { return img.generate(seed, 2048, 2048, &pool); });
        img.savePng();
    }
}