#include "AssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <lz4.h>

#include <json.hpp>

#include "../asset/Blueprint.hpp"
#include "absl/container/flat_hash_map.h"

namespace
{
template <typename T> T readFromInputFileStream(std::ifstream &stream)
{
    T result{};
    stream.read(reinterpret_cast<char *>(&result), sizeof(T));

    return result;
}

nlohmann::json readJsonFromInputFileStream(std::ifstream &stream, size_t size)
{
    std::string content;
    content.resize(size);
    stream.read(content.data(), size);

    return nlohmann::json::parse(content.begin(), content.end());
}

template <typename T> std::vector<T> convertBinaryBlobToVector(std::vector<char> &&binaryBlob)
{
    std::vector<T> data;
    data.resize(binaryBlob.size() / sizeof(T));
    memcpy(data.data(), binaryBlob.data(), binaryBlob.size());

    return data;
}

template <typename T> std::vector<char> convertVectorToBinaryBlob(std::vector<T> &&data)
{
    std::vector<char> binaryBlob;
    binaryBlob.resize(data.size() * sizeof(T));
    memcpy(binaryBlob.data(), data.data(), binaryBlob.size());

    return binaryBlob;
}
} // namespace

namespace pvk::io
{
std::unique_ptr<geometry::Object> loadObject(const std::filesystem::path &path)
{
    std::ifstream inputFile;
    inputFile.open(path, std::ios::binary);
    inputFile.seekg(0);

    const auto metadataSize = readFromInputFileStream<uint32_t>(inputFile);
    const auto blueprintJson = readJsonFromInputFileStream(inputFile, metadataSize);
    const auto matricesSize = readFromInputFileStream<uint32_t>(inputFile);

    std::vector<char> matricesBinaryBlob;
    matricesBinaryBlob.resize(matricesSize);
    inputFile.read(matricesBinaryBlob.data(), matricesSize);

    auto blueprint = pvk::asset::Blueprint::parseJson(blueprintJson);
    blueprint.matrices = convertBinaryBlobToVector<glm::mat4>(std::move(matricesBinaryBlob));

    absl::flat_hash_map<uint32_t, std::shared_ptr<geometry::Node>> nodeLookup{};

    for (auto &node : blueprint.nodes)
    {
        auto currentNode = std::make_shared<geometry::Node>(blueprint.matrices[node.identifier], std::move(node.meshIndices));

        if (node.identifier > -1)
        {
            currentNode->setParent(nodeLookup.at(node.identifier));
        }

        nodeLookup.insert(std::make_pair(node.identifier, std::move(currentNode)));
    }

    absl::flat_hash_map<uint32_t, std::unique_ptr<geometry::Mesh>> meshLookup{};

    for (size_t i = 0; i < blueprint.meshPaths.size(); i++) {
        auto mesh = std::make_unique<geometry::Mesh>(path.parent_path() / blueprint.meshPaths[i]);
        meshLookup.insert(std::make_pair(i, std::move(mesh)));
    }

    return std::make_unique<geometry::Object>(std::move(meshLookup), std::move(nodeLookup));
}

std::pair<std::vector<geometry::Vertex>, std::vector<uint32_t>> loadMeshBuffers(const std::filesystem::path &path)
{
    std::ifstream inputFile;
    inputFile.open(path, std::ios::binary);
    inputFile.seekg(0);

    std::string flags;
    flags.resize(4);
    inputFile.read(flags.data(), 4 * sizeof(char));

    auto metadataSize = readFromInputFileStream<uint32_t>(inputFile);

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

    std::vector<geometry::Vertex> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    vertexBuffer.resize(vertexSizeOriginal / sizeof(geometry::Vertex));
    indexBuffer.resize(indexSizeOriginal / sizeof(uint32_t));
    memcpy(vertexBuffer.data(), output.data(), vertexSizeOriginal);
    memcpy(indexBuffer.data(), output.data() + vertexSizeOriginal, indexSizeOriginal);

    return std::make_pair(vertexBuffer, indexBuffer);
}

} // namespace pvk::io