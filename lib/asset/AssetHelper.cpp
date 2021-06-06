#include "AssetHelper.hpp"

#include <lz4.h>

namespace pvk::asset
{
nlohmann::json readJsonFromInputFileStream(std::ifstream &stream, size_t size)
{
    std::string content;
    content.resize(size);
    stream.read(content.data(), size);

    return nlohmann::json::parse(content.begin(), content.end());
}

std::vector<char> compress(std::vector<char> &&uncompressedData)
{
    const auto compressedBound = LZ4_compressBound(uncompressedData.size());

    std::vector<char> compressedData;
    compressedData.resize(compressedBound);
    int compressedSize = LZ4_compress_default(uncompressedData.data(),
                                              compressedData.data(),
                                              static_cast<int>(uncompressedData.size()),
                                              static_cast<int>(compressedBound));
    compressedData.resize(compressedSize);

    return compressedData;
}

std::vector<char> uncompress(std::vector<char> &&compressedData, size_t uncompressedSize)
{
    std::vector<char> output;
    output.resize(uncompressedSize);

    LZ4_decompress_safe(compressedData.data(), output.data(), compressedData.size(), uncompressedSize);

    return output;
}
} // namespace pvk::asset