#pragma once

#include "bitmap.h"
#include "color.h"
#include "coord.h"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <BS_thread_pool.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <array>
#include <shared_mutex>

struct Bitmap24bitOptionalPixels {
    using value_type = std::optional<Color>;

    uint32_t width;
    uint32_t height;
    std::unique_ptr<value_type[]> data;

    Bitmap24bitOptionalPixels(uint32_t width, uint32_t height)
    : width{width}, height{height}, data{std::make_unique<value_type[]>(static_cast<size_t>(width) * height)} {}

    auto operator[](int i) const -> const value_type* { return &data[i * width]; }
    auto operator[](int i) -> value_type* { return &data[i * width]; }
    void reset() {
        for (size_t i = 0, wh = static_cast<size_t>(width) * height; i < wh; ++i) data[i].reset();
    }
};

class Omnichroma {
public:
    static Omnichroma Image32x16() { return Omnichroma{32, 16, 16, 8, 8}; }
    static Omnichroma Image64x64() { return Omnichroma{64, 64, 32, 32, 16}; }
    static Omnichroma Image256x128() { return Omnichroma{256, 128, 128, 64, 32}; }
    static Omnichroma Image512x512() { return Omnichroma{512, 512, 256, 256, 64}; }

    void generate(uint32_t seed, BS::thread_pool* pPool = nullptr) {
        this->seed = seed;
        this->pPool = pPool;
        // Create a pseudo-random bit generator from input seed
        std::mt19937 prbg{this->seed};
        // Clear image buffer, availble coords set
        buffer.reset();
        available = decltype(available){};
        // Generate random colors
        colors.clear();
        forEachEquidistantChannelValue([&](auto r) {
            forEachEquidistantChannelValue(
                [&](auto g) { forEachEquidistantChannelValue([&](auto b) { colors.emplace_back(r, g, b); }); });
        });
        std::shuffle(begin(colors), end(colors), prbg);
        // Place first color onto buffer
        placeColor(Coord{startX, startY}, colors.back());
        colors.pop_back();
        // Place remaining colors onto buffer
        size_t i = 1;
        for (const auto& color : colors)
            placeColor(bestCoord(color, [this](Coord coord, Color color) { return minDiffCoordScore(coord, color); }),
                       color);
    }

    void save() const {
        Bitmap24bitPixels image{static_cast<uint32_t>(imageW), static_cast<uint32_t>(imageH)};
        for (int i = 0; i < imageH; ++i)
            for (int j = 0; j < imageW; ++j) image[i][j] = *buffer[i][j];
        Bitmap24bit{image}.saveToFile(fmt::format("{}x{}-{}{}.bmp", imageW, imageH, seed, pPool ? "-mt" : ""));
    }

private:
    Omnichroma(int imageW, int imageH, int startX, int startY, int colorsPerChannel)
    : imageW{imageW},
      imageH{imageH},
      startX{startX},
      startY{startY},
      colorsPerChannel{colorsPerChannel},
      buffer{imageW, imageH} {}

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

    int minDiffCoordScore(Coord coord, Color color) {
        int res = 255 * 255 * 3 + 1;
        forEachNeighborCoord(coord, [&](Coord nb) {
            if (buffer[nb.y][nb.x]) res = std::min(res, sqDiff(color, *buffer[nb.y][nb.x]));
        });
        return res;
    }

    void placeColor(Coord coord, Color color) {
        std::unique_lock lock{mut};
        buffer[coord.y][coord.x] = color;
        available.erase(coord);
        forEachNeighborCoord(coord, [&](Coord neighborCoord) {
            if (!buffer[neighborCoord.y][neighborCoord.x]) available.insert(neighborCoord);
        });
    }

    template <class ScoreFunc>
    Coord bestCoord(Color color, ScoreFunc&& scoreFunc) {
        static int debugCnt = 0;
        if (pPool == nullptr) {
            return *std::min_element(begin(available), end(available), [&](const auto& lhs, const auto& rhs) {
                return scoreFunc(lhs, color) < scoreFunc(rhs, color);
            });
        }
        return bestCoord_mt(color, std::forward<ScoreFunc>(scoreFunc));
    }

    template <class ScoreFunc>
    Coord bestCoord_mt(Color color, ScoreFunc&& scoreFunc) {
        static int debugCnt = 0;
        constexpr size_t kMaxNumJobs = 16;
        constexpr size_t kMinItemsPerJob = 128;
        std::array<std::future<Coord>, kMaxNumJobs> jobs;
        size_t nJobs = std::clamp(available.size() / kMinItemsPerJob, size_t{1}, kMaxNumJobs);
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

        /*Coord best = jobs[0].get();
        for (size_t i = 1; i < nJobs; ++i) {
            Coord cur = jobs[i].get();
            if (comp(cur, best)) best = cur;
        }
        return best;*/
    }

    const int imageW;
    const int imageH;
    const int startX;
    const int startY;
    const int colorsPerChannel;
    Bitmap24bitOptionalPixels buffer;
    std::unordered_set<Coord> available;
    std::vector<Color> colors;
    uint32_t seed = 0;
    BS::thread_pool* pPool = nullptr;
    std::shared_mutex mut;
};