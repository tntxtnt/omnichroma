#pragma once

#include "bitmap.h"
#include "color.h"
#include "coord.h"
#include <BS_thread_pool.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_set>
#include <algorithm>
#include <array>
#include <vector>

struct Bitmap24bitOptionalPixels {
    using value_type = std::optional<Color>;

    uint32_t width;
    uint32_t height;
    std::unique_ptr<value_type[]> data;

    Bitmap24bitOptionalPixels(uint32_t width, uint32_t height);

    auto operator[](int i) const -> const value_type*;
    auto operator[](int i) -> value_type*;
    void reset();
};

class Omnichroma {
public:
    static Omnichroma Image32x16() { return Omnichroma{32, 16, 8}; }
    static Omnichroma Image64x64() { return Omnichroma{64, 64, 16}; }
    static Omnichroma Image256x128() { return Omnichroma{256, 128, 32}; }
    static Omnichroma Image512x512() { return Omnichroma{512, 512, 64}; }
    static Omnichroma Image2048x1024() { return Omnichroma{2048, 1024, 128}; }
    static Omnichroma Image4096x4096() { return Omnichroma{4096, 4096, 256}; }

    void generate(uint32_t seed, int startX, int startY, BS::thread_pool* pPool = nullptr);
    auto outputFilename() const -> std::string;
    void saveBmp() const;
    void savePng() const;

private:
    Omnichroma(int imageW, int imageH, int colorsPerChannel);

    auto minDiffCoordScore(Coord coord, Color color) -> int;
    void placeColor(Coord coord, Color color);

    template <class Func>
    void forEachEquidistantChannelValue(Func&& yield) {
        for (int i = 0; i < colorsPerChannel; ++i) yield(static_cast<uint8_t>(i * 255 / (colorsPerChannel - 1)));
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
            jobs[i] = pPool->submit([this, first, last, &comp] { return *std::min_element(first, last, comp); });
            first = last;
        }

        std::array<Coord, kMaxNumJobs> doneJobs;
        std::transform(begin(jobs), begin(jobs) + nJobs, begin(doneJobs), [](auto& job) { return job.get(); });
        return *std::min_element(begin(doneJobs), begin(doneJobs) + nJobs, comp);
    }

    const int imageW;
    const int imageH;
    const int colorsPerChannel;
    Bitmap24bitOptionalPixels buffer;
    std::unordered_set<Coord> available;
    std::vector<Color> colors;
    uint32_t seed = 0;
    int startX = 0;
    int startY = 0;
    BS::thread_pool* pPool = nullptr;
};
