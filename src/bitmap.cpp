#include "bitmap.h"

#include <cstring> // std::memcpy
#include <fmt/ostream.h>
#include <iostream>
#include <fstream>

Bitmap24bitPixels::Bitmap24bitPixels(uint32_t width, uint32_t height)
: width{width}, height{height}, data{std::make_unique<Color[]>(width * height)} {}

auto Bitmap24bitPixels ::operator[](int i) const -> const Color* {
    return &data[i * width];
}
auto Bitmap24bitPixels ::operator[](int i) -> Color* {
    return &data[i * width];
}

Bitmap24bit::Bitmap24bit(const Bitmap24bitPixels& image) {
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

auto Bitmap24bit::saveToFile(std::string_view filename) const -> bool {
    if (std::ofstream ofs{filename.data(), std::ios::binary}) {
        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        ofs.write(reinterpret_cast<const char*>(&dib), sizeof(dib));
        ofs.write(reinterpret_cast<const char*>(data.get()), dib.imageByteSize);
        return true;
    }
    fmt::print(std::cerr, "Cannot open `out.bmp`\n");
    return false;
}
