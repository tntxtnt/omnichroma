#pragma once

#include "color.h"
#include <fmt/ostream.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <cstring> // std::memcpy

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
    Bitmap24bitPixels(uint32_t width, uint32_t height)
    : width{width}, height{height}, data{std::make_unique<Color[]>(width * height)} {}
    auto operator[](int i) const -> const Color* { return &data[i * width]; }
    auto operator[](int i) -> Color* { return &data[i * width]; }
};

struct Bitmap24bit {
    BitmapFileHeader header{{'B', 'M'}, 54, 0, 0, 54};
    BitmapInformationHeader dib{};
    std::unique_ptr<uint32_t[]> data = nullptr;

    Bitmap24bit(const Bitmap24bitPixels& image) {
        dib.imageWidth = image.width;
        dib.imageHeight = image.height;
        const uint32_t rowByteSize = dib.imageWidth * sizeof(Color);
        const uint32_t rowSize = rowByteSize / 4 + static_cast<int>(rowByteSize % 4 != 0);
        const uint32_t dataSize = rowSize * dib.imageHeight;
        dib.imageByteSize = dataSize * sizeof(uint32_t);
        header.size += dib.imageByteSize;
        data = std::make_unique<uint32_t[]>(dataSize);
        for (uint32_t i = 0; i < dib.imageHeight; ++i)
            std::memcpy(&data[i * rowSize], image[dib.imageHeight - 1 - i], rowByteSize);
    }

    auto saveToFile(std::string_view filename) const -> bool {
        if (std::ofstream ofs{filename.data(), std::ios::binary}) {
            ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
            ofs.write(reinterpret_cast<const char*>(&dib), sizeof(dib));
            ofs.write(reinterpret_cast<const char*>(data.get()), dib.imageByteSize);
            return true;
        }
        fmt::print(std::cerr, "Cannot open `out.bmp`\n");
        return false;
    }
};
