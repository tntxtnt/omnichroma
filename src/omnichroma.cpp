#include "omnichroma.h"

#include <fmt/ostream.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <lodepng.h>
#include <iostream>
#include <random>
#include <BS_thread_pool.hpp>

Bitmap24bitOptionalPixels::Bitmap24bitOptionalPixels(uint32_t width, uint32_t height)
: width{width}, height{height}, data{std::make_unique<value_type[]>(static_cast<size_t>(width) * height)} {}

auto Bitmap24bitOptionalPixels::operator[](int i) const -> const value_type* {
    return &data[i * width];
}

auto Bitmap24bitOptionalPixels::operator[](int i) -> value_type* {
    return &data[i * width];
}

void Bitmap24bitOptionalPixels::reset() {
    for (size_t i = 0, wh = static_cast<size_t>(width) * height; i < wh; ++i) data[i].reset();
}

void Omnichroma::generate(uint32_t seed, int startX, int startY, BS::thread_pool* pPool) {
    this->seed = seed;
    this->pPool = pPool;
    this->startX = startX;
    this->startY = startY;
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

auto Omnichroma::outputFilename() const -> std::string {
    return fmt::format("{}-{}x{}-{}xy{}{}", seed, imageW, imageH, startX, startY, pPool ? "-mt" : "");
}

void Omnichroma::saveBmp() const {
    Bitmap24bitPixels image{static_cast<uint32_t>(imageW), static_cast<uint32_t>(imageH)};
    for (int i = 0; i < imageH; ++i)
        for (int j = 0; j < imageW; ++j) image[i][j] = *buffer[i][j];
    Bitmap24bit{image}.saveToFile(outputFilename() + ".bmp");
}

void Omnichroma::savePng() const {
    std::vector<unsigned char> image(size_t{3} * imageW * imageH);
    for (int i = 0; i < imageH; ++i)
        for (int j = 0; j < imageW; ++j) {
            image[3 * (imageW * i + j) + 0] = buffer[i][j]->r;
            image[3 * (imageW * i + j) + 1] = buffer[i][j]->g;
            image[3 * (imageW * i + j) + 2] = buffer[i][j]->b;
        }
    unsigned error = lodepng::encode(outputFilename() + ".png", image, imageW, imageH, LCT_RGB);
    if (error) fmt::print(std::cerr, "lodepng encoder error {}: {}\n", error, lodepng_error_text(error));
}

Omnichroma::Omnichroma(int imageW, int imageH, int colorsPerChannel)
: imageW{imageW}, imageH{imageH}, colorsPerChannel{colorsPerChannel}, buffer{imageW, imageH} {}

auto Omnichroma::minDiffCoordScore(Coord coord, Color color) -> int {
    int res = 255 * 255 * 3 + 1;
    forEachNeighborCoord(coord, [&](Coord nb) {
        if (buffer[nb.y][nb.x]) res = std::min(res, sqDiff(color, *buffer[nb.y][nb.x]));
    });
    return res;
}

void Omnichroma::placeColor(Coord coord, Color color) {
    buffer[coord.y][coord.x] = color;
    available.erase(coord);
    forEachNeighborCoord(coord, [&](Coord neighborCoord) {
        if (!buffer[neighborCoord.y][neighborCoord.x]) available.insert(neighborCoord);
    });
}