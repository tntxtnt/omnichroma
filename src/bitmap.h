#pragma once

#include "color.h"
#include <cstdint>
#include <memory>
#include <string>

#pragma pack(push, 2)
struct BitmapFileHeader {
    char headerField[2]{'B', 'M'};
    uint32_t size = 14;
    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;
    uint32_t pixelArrayOffset = 14;
};
#pragma pack(pop)

struct BitmapInformationHeader {
    uint32_t size = 40;
    uint32_t imageWidth = 0;
    uint32_t imageHeight = 0;
    uint16_t numColorPlanes = 1;
    uint16_t bitsPerPixel = 24;
    uint32_t compressionMethod = 0;
    uint32_t imageByteSize = 0;
    uint32_t horizontalResolution = 0;
    uint32_t verticalResolution = 0;
    uint32_t numColors = 0;
    uint32_t numImportantColors = 0;
};

struct Bitmap24bitPixels {
    uint32_t width;
    uint32_t height;
    std::unique_ptr<Color[]> data;
    Bitmap24bitPixels(uint32_t width, uint32_t height);
    auto operator[](int i) const -> const Color*;
    auto operator[](int i) -> Color*;
};

struct Bitmap24bit {
    BitmapFileHeader header{{'B', 'M'}, 54, 0, 0, 54};
    BitmapInformationHeader dib{};
    std::unique_ptr<uint32_t[]> data = nullptr;

    Bitmap24bit(const Bitmap24bitPixels& image);

    auto saveToFile(std::string_view filename) const -> bool;
};
