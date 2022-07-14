#pragma once

#include "bitmap.h"
#include "pixelsbuffer.h"
#include "color.h"
#include "coord.h"
#include "generate.h"
#include <BS_thread_pool.hpp>
#include <lodepng.h>
#include <fmt/ostream.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <cstdint>
#include <unordered_set>
#include <algorithm>
#include <array>
#include <vector>
#include <iostream>
#include <random>
#include <boost/predef.h>

template <class CoordSet = std::unordered_set<Coord>>
class Omnichroma {
public:
    static Omnichroma Image8x8() { return Omnichroma{8, 8, 4}; }
    static Omnichroma Image32x16() { return Omnichroma{32, 16, 8}; }
    static Omnichroma Image64x64() { return Omnichroma{64, 64, 16}; }
    static Omnichroma Image256x128() { return Omnichroma{256, 128, 32}; }
    static Omnichroma Image512x512() { return Omnichroma{512, 512, 64}; }
    static Omnichroma Image2048x1024() { return Omnichroma{2048, 1024, 128}; }
    static Omnichroma Image4096x4096() { return Omnichroma{4096, 4096, 256}; }

    void generate(uint32_t seed, uint32_t startX, uint32_t startY, BS::thread_pool* pPool = nullptr) {
        this->seed = seed;
        this->pPool = pPool;
        this->startX = startX;
        this->startY = startY;
        // Create a uniform random bit generator from input seed
        std::mt19937 urbg{this->seed};
        // Clear image buffer, availble coords set
        buffer.reset();
        available = decltype(available){};
        // Generate random colors
        generateShuffledColors<Color>(colorsPerChannel, urbg).swap(colors);
        // Place first color onto buffer
        placeColor(Coord{startX, startY}, colors.back());
        colors.pop_back();
        // Place remaining colors onto buffer
        size_t i = 1;
        for (const auto& color : colors)
            placeColor(bestCoord(color, [this](Coord coord, Color color) { return minDiffCoordScore(coord, color); }),
                       color);
    }

    auto getFilename() const -> std::string {
        return fmt::format("{}-{}x{}-{}xy{}-{}-{}", seed, imageW, imageH, startX, startY, getPlatformName(),
                           getArchName());
    }

    void saveBmp(std::string_view filename = "") const {
        Bitmap24bitPixels image{static_cast<uint32_t>(imageW), static_cast<uint32_t>(imageH)};
        for (int i = 0; i < imageH; ++i)
            for (int j = 0; j < imageW; ++j) image[i][j] = *buffer[i][j];
        const std::string outFileName = filename.empty() ? outputFilename() + ".bmp" : std::string{filename};
        Bitmap24bit{image}.saveToFile(outFileName);
    }

    void savePng(std::string_view filename = "") const {
        std::vector<unsigned char> image(size_t{3} * imageW * imageH);
        for (int i = 0; i < imageH; ++i)
            for (int j = 0; j < imageW; ++j) {
                image[3 * (imageW * i + j) + 0] = buffer[i][j]->r;
                image[3 * (imageW * i + j) + 1] = buffer[i][j]->g;
                image[3 * (imageW * i + j) + 2] = buffer[i][j]->b;
            }
        const std::string outFileName = filename.empty() ? outputFilename() + ".png" : std::string{filename};
        unsigned error = lodepng::encode(outFileName, image, imageW, imageH, LCT_RGB);
        if (error) fmt::print(std::cerr, "lodepng encoder error {}: {}\n", error, lodepng_error_text(error));
    }

private:
    Omnichroma(uint32_t imageW, uint32_t imageH, uint32_t colorsPerChannel)
    : imageW{imageW}, imageH{imageH}, colorsPerChannel{colorsPerChannel}, buffer{imageW, imageH} {}

    auto outputFilename() const -> std::string { return fmt::format("{}{}", getFilename(), pPool ? "-mt" : ""); }

    static auto getPlatformName() -> std::string {
        if constexpr (static_cast<bool>(BOOST_COMP_CLANG)) return "cla";
        if constexpr (static_cast<bool>(BOOST_COMP_MSVC)) return "mvc";
        if constexpr (static_cast<bool>(BOOST_PLAT_MINGW)) return "mgw";
        if constexpr (static_cast<bool>(BOOST_COMP_GNUC)) return "gcc";
        return "unk";
    }

    static auto getArchName() -> std::string {
        if constexpr (static_cast<bool>(BOOST_ARCH_X86_64)) return "64";
        if constexpr (static_cast<bool>(BOOST_ARCH_X86_32)) return "32";
        return "00";
    }

    auto minDiffCoordScore(Coord coord, Color color) -> int {
        int res = 255 * 255 * 3 + 1;
        forEachNeighborCoord(coord, [&](Coord nb) {
            if (buffer[nb.y][nb.x]) res = std::min(res, sqDiff(color, *buffer[nb.y][nb.x]));
        });
        return res;
    }

    void placeColor(Coord coord, Color color) {
        buffer[coord.y][coord.x] = color;
        available.erase(coord);
        forEachNeighborCoord(coord, [&](Coord neighborCoord) {
            if (!buffer[neighborCoord.y][neighborCoord.x]) available.insert(neighborCoord);
        });
    }

    template <class Func>
    void forEachEquidistantChannelValue(Func&& yield) {
        for (uint32_t i = 0; i < colorsPerChannel; ++i) yield(static_cast<uint8_t>(i * 255 / (colorsPerChannel - 1)));
    }
    template <class Func>
    void forEachNeighborCoord(Coord coord, Func&& yield) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (coord.y + dy == -1 || coord.y + dy == imageH) continue;
            for (int dx = -1; dx <= 1; ++dx) {
                if (coord.x + dx == -1 || coord.x + dx == imageW) continue;
                yield(Coord{coord.x + dx, coord.y + dy});
            }
        }
    }
    template <class ScoreFunc>
    auto bestCoord(Color color, ScoreFunc&& scoreFunc) -> Coord {
        return pPool ? bestCoord_mt(color, std::forward<ScoreFunc>(scoreFunc))
                     : *std::min_element(begin(available), end(available), [&](const auto& lhs, const auto& rhs) {
                           return scoreFunc(lhs, color) < scoreFunc(rhs, color);
                       });
    }
    template <class ScoreFunc>
    auto bestCoord_mt(Color color, ScoreFunc&& scoreFunc) -> Coord {
        constexpr size_t kMaxNumJobs = 16;
        constexpr size_t kMinItemsPerJob = 256;

        std::array<std::future<Coord>, kMaxNumJobs> jobs;
        const size_t nJobs =
            std::clamp(available.size() / kMinItemsPerJob, size_t{1}, static_cast<size_t>(pPool->get_thread_count()));
        const size_t itemsPerJob = available.size() / nJobs;

        auto first = begin(available);
        auto comp = [&](const auto& lhs, const auto& rhs) { return scoreFunc(lhs, color) < scoreFunc(rhs, color); };
        for (size_t i = 0; i < nJobs; ++i) {
            auto last = first;
            if (i == nJobs - 1) {
                last = end(available);
            } else {
                std::advance(last, itemsPerJob);
            }
            jobs[i] = pPool->submit([first, last, &comp] { return *std::min_element(first, last, comp); });
            first = last;
        }

        std::array<Coord, kMaxNumJobs> doneJobs;
        std::transform(begin(jobs), begin(jobs) + nJobs, begin(doneJobs), [](auto& job) { return job.get(); });
        return *std::min_element(begin(doneJobs), begin(doneJobs) + nJobs, comp);
    }

    const uint32_t imageW;
    const uint32_t imageH;
    const uint32_t colorsPerChannel;
    Bitmap24bitOptionalPixels buffer;
    CoordSet available;
    std::vector<Color> colors;
    uint32_t seed = 0;
    uint32_t startX = 0;
    uint32_t startY = 0;
    BS::thread_pool* pPool = nullptr;
};
