#include "AssetLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <span>

#include <absl/container/flat_hash_map.h>

#include <lz4.h>

#include <json.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "io.hpp"

#include "../asset/AssetHelper.hpp"
#include "../asset/Blueprint.hpp"
#include "../engine/Graphics.hpp"
#include "../engine/shader/Texture.hpp"
#include "../geometry/Material.hpp"
#include "../geometry/Mesh.hpp"
#include "../vulkan/Helper.hpp"

namespace
{
absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Node>> getNodeLookup(
    pvk::asset::Blueprint &blueprint,
    const absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> &meshLookup,
    const absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Material>> &materialLookup)
{
    absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Node>> nodeLookup{};

    for (auto &node : blueprint.nodes)
    {
        std::vector<std::weak_ptr<pvk::geometry::Mesh>> meshes{};

        for (size_t i = 0; i < node.meshIndices.size(); i++)
        {
            const auto meshIndex = node.meshIndices[i];
            const auto materialIndex = node.materialIndices[i];

            meshLookup.at(meshIndex)->setMaterial(materialLookup.at(materialIndex));
            meshes.emplace_back(meshLookup.at(meshIndex));
        }

        auto currentNode = std::make_shared<pvk::geometry::Node>(
            std::move(node.name), blueprint.matrices[node.identifier], std::move(meshes));

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

std::pair<glm::vec3, glm::vec3> calculateBounds(const std::vector<pvk::geometry::Vertex> &vertices)
{
    const auto x = std::minmax_element(
        vertices.begin(), vertices.end(), [](const pvk::geometry::Vertex &a, const pvk::geometry::Vertex &b) {
            return a.position.x < b.position.x;
        });

    const auto y = std::minmax_element(
        vertices.begin(), vertices.end(), [](const pvk::geometry::Vertex &a, const pvk::geometry::Vertex &b) {
            return a.position.y < b.position.y;
        });

    const auto z = std::minmax_element(
        vertices.begin(), vertices.end(), [](const pvk::geometry::Vertex &a, const pvk::geometry::Vertex &b) {
            return a.position.z < b.position.z;
        });

    return std::make_pair(glm::vec3(x.first->position.x, y.first->position.y, z.first->position.z),
                          glm::vec3(x.second->position.x, y.second->position.y, z.second->position.z));
}

absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> getMeshLookup(pvk::asset::Blueprint &blueprint,
                                                                                  const std::filesystem::path &path)
{
    absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Mesh>> meshLookup{};

    for (size_t i = 0; i < blueprint.meshPaths.size(); i++)
    {
        auto [vertices, indices] = pvk::io::loadMeshBuffers(path.parent_path() / blueprint.meshPaths[i]);
        const auto numVertices = vertices.size();
        const auto numIndices = indices.size();

        auto vertexBuffer = pvk::vulkan::createBuffer(
            std::move(vertices), vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
        auto indexBuffer = pvk::vulkan::createBuffer(
            std::move(indices), vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);

        auto mesh = std::make_shared<pvk::geometry::Mesh>(
            std::move(vertexBuffer), std::move(indexBuffer), numVertices, numIndices, calculateBounds(vertices));

        meshLookup.insert(std::make_pair(i, std::move(mesh)));
    }

    return meshLookup;
}

absl::flat_hash_map<std::string, std::shared_ptr<pvk::engine::Texture>> getTextureLookup(pvk::asset::Blueprint &blueprint,
                                                                                      const std::filesystem::path &path)
{
    absl::flat_hash_map<std::string, std::shared_ptr<pvk::engine::Texture>> textureLookup{};

    auto currentTexture = 0;

    for (const auto &material : blueprint.materials)
    {
        for (const auto &[index, texture] : material.textureData)
        {
            textureLookup.insert({index, pvk::io::loadTexture(path.parent_path() / texture)});
            currentTexture++;
        }
    }

    return textureLookup;
}

absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Material>> getMaterialLookup(
    pvk::asset::Blueprint &blueprint)
{
    absl::flat_hash_map<uint32_t, std::shared_ptr<pvk::geometry::Material>> materialLookup{};

    for (auto &material : blueprint.materials)
    {
        materialLookup.insert(std::make_pair(
            material.identifier,
            std::make_shared<pvk::geometry::Material>(std::move(material.name), std::move(material.customData))));
    }

    return materialLookup;
}
} // namespace

namespace pvk::io
{
std::shared_ptr<geometry::Object> loadObject(const std::filesystem::path &path)
{
    std::ifstream inputFile;
    inputFile.open(path, std::ios::binary);
    inputFile.seekg(0);

    const auto metadataSize = pvk::asset::readFromInputFileStream<uint32_t>(inputFile);
    const auto blueprintJson = pvk::asset::readMessagePackFromInputFileStream(inputFile, metadataSize);
    const auto matricesSize = pvk::asset::readFromInputFileStream<uint32_t>(inputFile);

    auto blueprint = pvk::asset::Blueprint::parseJson(blueprintJson);
    blueprint.matrices = pvk::asset::convertBinaryToVector<glm::mat4>(
        pvk::asset::readFromInputFileStream<std::vector<char>>(inputFile, matricesSize));

    auto textureLookup = getTextureLookup(blueprint, path);
    auto materialLookup = getMaterialLookup(blueprint);
    auto meshLookup = getMeshLookup(blueprint, path);
    auto nodeLookup = getNodeLookup(blueprint, meshLookup, materialLookup);

    return std::make_unique<geometry::Object>(
        std::move(meshLookup), std::move(nodeLookup), std::move(materialLookup), std::move(textureLookup));
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

std::shared_ptr<engine::Texture> loadTexture(const std::filesystem::path &path)
{
    int width, height, c;
    const auto textureBuffer = stbi_load(path.c_str(), &width, &height, &c, STBI_rgb_alpha);
    
    auto texture = std::make_shared<engine::Texture>(height * width * STBI_rgb_alpha, width, height);
    texture->update(textureBuffer);

    return texture;
}

} // namespace pvk::io