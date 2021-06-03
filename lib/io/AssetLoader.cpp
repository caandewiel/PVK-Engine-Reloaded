#include "AssetLoader.hpp"

#include <fstream>
#include <iostream>
#include <span>
#include <utility>
#include <vector>

#include <lz4.h>

#include <json.hpp>

namespace pvk::io
{
std::pair<std::vector<char>, std::vector<char>> loadVertexBinary(const std::string &path)
{
    std::ifstream inputFile;
    inputFile.open(path, std::ios::binary);
    inputFile.seekg(0);

    uint32_t metadataSize = 0;
    inputFile.read(reinterpret_cast<char *>(&metadataSize), sizeof(uint32_t));

    std::string metadataContent;
    metadataContent.resize(metadataSize);
    inputFile.read(metadataContent.data(), metadataSize);

    auto metadata = nlohmann::json::parse(metadataContent.begin(), metadataContent.end());

    const auto vertexSizeOriginal = metadata["vertexSizeOriginal"].get<uint32_t>();
    const auto indexSizeOriginal = metadata["indexSizeOriginal"].get<uint32_t>();
    const auto targetSize = vertexSizeOriginal + indexSizeOriginal;
    const auto compressedSize = metadata["compressedSize"].get<uint32_t>();

    std::string binaryBlob;
    binaryBlob.resize(compressedSize);
    inputFile.read(binaryBlob.data(), compressedSize);

    std::string output;
    output.resize(targetSize);

    LZ4_decompress_safe(binaryBlob.data(), output.data(), compressedSize, targetSize);

    std::vector<char> vertexBuffer;
    std::vector<char> indexBuffer;
    vertexBuffer.resize(vertexSizeOriginal);
    indexBuffer.resize(indexSizeOriginal);
    memcpy(vertexBuffer.data(), output.data(), vertexSizeOriginal);
    memcpy(indexBuffer.data(), output.data() + vertexSizeOriginal, indexSizeOriginal);

    return std::make_pair(vertexBuffer, indexBuffer);
}

} // namespace pvk::io