#include "omnichroma.h"
#include <fmt/format.h>
#include <chrono>
#include <string>
#include <random>
#include <tuple>
#include <optional>
#include <numeric>

#include <BS_thread_pool.hpp>

#include <unordered_set>
#include <boost/unordered_set.hpp>
#include <tsl/hopscotch_set.h>
#include <tsl/ordered_set.h>
#include <tsl/sparse_set.h>
#include <tsl/robin_set.h>

#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

template <class Func>
auto timeit(Func&& f) {
    const auto startTime = std::chrono::steady_clock::now();
    f();
    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - startTime;
    fmt::print("Total = {}s\n", elapsed.count());
}

std::pair<unsigned int, unsigned int> getDimension(unsigned int dimensionId) {
    switch (dimensionId) {
    case 0: return std::make_pair(8u, 8u);
    case 1: return std::make_pair(32u, 16u);
    case 2: return std::make_pair(64u, 64u);
    case 3: return std::make_pair(256u, 128u);
    case 4: return std::make_pair(512u, 512u);
    case 5: return std::make_pair(2048u, 1024u);
    case 6: return std::make_pair(4096u, 4096u);
    default: return std::make_pair(0u, 0u);
    }
}

template <class OmnichromaType>
OmnichromaType construct(unsigned dimensionId) {
    switch (dimensionId) {
    case 0: return OmnichromaType::Image8x8();
    case 1: return OmnichromaType::Image32x16();
    case 2: return OmnichromaType::Image64x64();
    case 3: return OmnichromaType::Image256x128();
    case 4: return OmnichromaType::Image512x512();
    case 5: return OmnichromaType::Image2048x1024();
    default: return OmnichromaType::Image4096x4096();
    }
}

template <class CoordSet = std::unordered_set<Coord>>
void generate(unsigned seed, unsigned dimensionId, unsigned startX, unsigned startY, std::string_view coordSetName) {
    using omnichroma_t = Omnichroma<CoordSet>;
    timeit([=] {
        auto img = construct<omnichroma_t>(dimensionId);
        if (dimensionId < 4) { // don't use thread pool for small images
            img.generate(seed, startX, startY);
        } else {
            BS::thread_pool pool;
            fmt::print("Num threads = {}\n", pool.get_thread_count());
            img.generate(seed, startX, startY, &pool);
        }
        img.savePng(fmt::format("{}-{}.png", img.getFilename(), coordSetName));
    });
}

auto main(int argc, char** argv) -> int {
    unsigned int seed = std::random_device{}();
    unsigned int dimensionId = 3;
    std::optional<unsigned int> startX;
    std::optional<unsigned int> startY;
    unsigned int coordSetTypeId = 3;

    CLI::App app{"Omnichroma - generate image with all distinct colors"};
    app.add_option("-s,--seed", seed, "Seed, default to a random value");
    app.add_option("-d,--dimension", dimensionId, R"(Image dimension
  6 4096x4096
  5 2048x1024
  4 512x512
  3 256x128 (default)
  2 64x64
  1 32x16
  0 8x8)");
    app.add_option("-x,--start-x", startX, "x coord of starting point, default to middle of image");
    app.add_option("-y,--start-y", startY, "y coord of starting point, default to middle of image");
    app.add_option("-c,--coord-set-type", coordSetTypeId, R"(Available coords set type
  0 std::unordered_set
  1 boost::unordered_set
  2 tsl::hopscotch_set
  3 tsl::ordered_set (default)
  4 tsl::sparse_set
  5 tsl::robin_set)");
    CLI11_PARSE(app, argc, argv);

    if (dimensionId > 6) {
        fmt::print(std::cerr, "--dimension must be an UINT <= 6 (got {})\n", dimensionId);
        return 1;
    }
    if (coordSetTypeId > 5) {
        fmt::print(std::cerr, "--coord-set-type must be an UINT <= 5 (got {})\n", coordSetTypeId);
        return 1;
    }
    const auto [imgW, imgH] = getDimension(dimensionId);
    if (!startX) {
        startX = imgW / 2;
    } else if (*startX >= imgW) {
        fmt::print(std::cerr, "--start-x must be an UINT <= {} (got {})\n", imgW, *startX);
        return 1;
    }
    if (!startY) {
        startY = imgH / 2;
    } else if (*startY >= imgH) {
        fmt::print(std::cerr, "--start-y must be an UINT <= {} (got {})\n", imgH, *startY);
        return 1;
    }

    switch (coordSetTypeId) {
    case 1: //
        generate<boost::unordered_set<Coord, std::hash<Coord>>>(seed, dimensionId, *startX, *startY, "bus");
        break;
    case 2: //
        generate<tsl::hopscotch_set<Coord>>(seed, dimensionId, *startX, *startY, "hop");
        break;
    case 3: //
        generate<tsl::ordered_set<Coord>>(seed, dimensionId, *startX, *startY, "ord");
        break;
    case 4: //
        generate<tsl::sparse_set<Coord>>(seed, dimensionId, *startX, *startY, "spa");
        break;
    case 5: //
        generate<tsl::robin_set<Coord>>(seed, dimensionId, *startX, *startY, "rob");
        break;
    default: //
        generate<std::unordered_set<Coord>>(seed, dimensionId, *startX, *startY, "sus");
        break;
    }
}