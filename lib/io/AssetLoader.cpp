#include "AssetLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include <lz4.h>

#include <json.hpp>

#include "../asset/AssetHelper.hpp"
#include "../asset/Blueprint.hpp"
#include "../geometry/Mesh.hpp"

namespace
{
absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Node>> getNodeLookup(
    pvk::asset::Blueprint &blueprint,
    const absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> &meshLookup)
{
    absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Node>> nodeLookup{};

    for (auto &node : blueprint.nodes)
    {
        std::vector<std::weak_ptr<pvk::geometry::Mesh>> meshes{};

        for (auto meshIndex : node.meshIndices) {
            meshes.emplace_back(meshLookup.at(meshIndex));
        }

        auto currentNode =
            std::make_shared<pvk::geometry::Node>(blueprint.matrices[node.identifier], std::move(meshes));

        if (node.parent > -1)
        {
            currentNode->setParent(nodeLookup.at(node.parent));
        }

        nodeLookup.insert(std::make_pair(node.identifier, std::move(currentNode)));
    }

    for (auto &[index, node] : nodeLookup)
    {
        if (node->hasParent())
        {
            node->getParent().addChild(nodeLookup.at(index));
        }
    }

    return nodeLookup;
}

absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> getMeshLookup(pvk::asset::Blueprint &blueprint,
                                                                                  const std::filesystem::path &path)
{
    absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> meshLookup{};

    for (size_t i = 0; i < blueprint.meshPaths.size(); i++)
    {
        auto mesh = std::make_shared<pvk::geometry::Mesh>(path.parent_path() / blueprint.meshPaths[i]);
        meshLookup.insert(std::make_pair(i, std::move(mesh)));
    }

    return meshLookup;
}
} // namespace

namespace pvk::io
{
std::unique_ptr<geometry::Object> loadObject(const std::filesystem::path &path)
{
    std::ifstream inputFile;
    inputFile.open(path, std::ios::binary);
    inputFile.seekg(0);

    const auto metadataSize = pvk::asset::readFromInputFileStream<uint32_t>(inputFile);
    const auto blueprintJson = pvk::asset::readJsonFromInputFileStream(inputFile, metadataSize);
    const auto matricesSize = pvk::asset::readFromInputFileStream<uint32_t>(inputFile);

    auto blueprint = pvk::asset::Blueprint::parseJson(blueprintJson);
    blueprint.matrices = pvk::asset::convertBinaryToVector<glm::mat4>(
        pvk::asset::readFromInputFileStream<std::vector<char>>(inputFile, matricesSize));
    
    auto meshLookup = getMeshLookup(blueprint, path);
    auto nodeLookup = getNodeLookup(blueprint, meshLookup);

    return std::make_unique<geometry::Object>(std::move(meshLookup), std::move(nodeLookup));
}

std::pair<std::vector<geometry::Vertex>, std::vector<uint32_t>> loadMeshBuffers(const std::filesystem::path &path)
{
    std::ifstream inputFile;
    inputFile.open(path, std::ios::binary);
    inputFile.seekg(0);

    const auto flags = pvk::asset::readFromInputFileStream<std::vector<char>>(inputFile, 4);

    const auto metadataSize = pvk::asset::readFromInputFileStream<uint32_t>(inputFile);
    const auto metadataContent = pvk::asset::readFromInputFileStream<std::vector<char>>(inputFile, metadataSize);
    const auto metadata = nlohmann::json::parse(metadataContent.begin(), metadataContent.end());

    const auto vertexSizeOriginal = metadata["vertexSizeOriginal"].get<uint32_t>();
    const auto indexSizeOriginal = metadata["indexSizeOriginal"].get<uint32_t>();
    const auto targetSize = vertexSizeOriginal + indexSizeOriginal;
    const auto compressedSize = metadata["compressedSize"].get<uint32_t>();

    auto binaryBlob = pvk::asset::readFromInputFileStream<std::vector<char>>(inputFile, compressedSize);
    const auto output = pvk::asset::uncompress(std::move(binaryBlob), targetSize);

    std::vector<geometry::Vertex> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    vertexBuffer.resize(vertexSizeOriginal / sizeof(geometry::Vertex));
    indexBuffer.resize(indexSizeOriginal / sizeof(uint32_t));
    memcpy(vertexBuffer.data(), output.data(), vertexSizeOriginal);
    memcpy(indexBuffer.data(), output.data() + vertexSizeOriginal, indexSizeOriginal);

    return std::make_pair(vertexBuffer, indexBuffer);
}

} // namespace pvk::io