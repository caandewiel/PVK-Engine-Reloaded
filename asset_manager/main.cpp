// Insipred by: https://vkguide.dev/docs/extra-chapter/asset_system/

#include "assimp/mesh.h"
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <fmt/format.h>

#include <lz4.h>

#include <json.hpp>

#include <glm/glm.hpp>

constexpr auto assimpFlags = aiProcess_OptimizeMeshes | aiProcess_GenNormals | aiProcess_FlipUVs;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct AssetFile {
    std::string metadata;
    std::vector<char> binaryBlob;
};

namespace
{
void writeJsonToBinary()
{
    nlohmann::json metadata;
    metadata["nVertex"] = 3;
    metadata["binaryVertexSize"] = 12;
    auto content = metadata.dump();

    auto compressedBound = LZ4_compressBound(content.size());
    std::vector<char> binaryBlob;
    binaryBlob.resize(compressedBound);
    int compressedSize = LZ4_compress_default(
        content.data(), binaryBlob.data(), static_cast<int>(content.size()), static_cast<int>(compressedBound));
    binaryBlob.resize(compressedSize);

    std::ofstream binaryFile;
    binaryFile.open("/Users/christian/test.bin", std::ios::binary | std::ios::out);

    uint32_t length = content.size();
    binaryFile.write(reinterpret_cast<const char *>(&length), sizeof(uint32_t));
    binaryFile.write(reinterpret_cast<const char *>(&compressedSize), sizeof(uint32_t));
    binaryFile.write(binaryBlob.data(), binaryBlob.size());
    binaryFile.close();
}

void loadJson()
{
    std::ifstream inputFile;
    inputFile.open(fmt::format("{}/{}.pvm", "/Users/christian", "Beta_Surface"), std::ios::binary);
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
    binaryBlob.resize(targetSize);
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
    auto vertices = std::span(reinterpret_cast<Vertex *>(vertexBuffer.data()), vertexSizeOriginal / sizeof(Vertex));
    auto indices = std::span(reinterpret_cast<uint32_t *>(indexBuffer.data()), indexSizeOriginal / sizeof(uint32_t));
    auto vertex = vertices[0];

    std::cout << fmt::format("{}, {}, {}", vertex.position[0], vertex.position[1], vertex.position[2]) << std::endl;
    std::cout << fmt::format("{}, {}, {}", indices[0], indices[1], indices[2]) << std::endl;
}

void loadMeshes(const aiScene *scene)
{
    const auto meshes = std::span(scene->mMeshes, scene->mNumMeshes);

    for (const auto &mesh : meshes)
    {
        std::vector<Vertex> vertexBuffer;
        std::vector<uint32_t> indexBuffer;
        vertexBuffer.reserve(mesh->mNumVertices);
        indexBuffer.reserve(mesh->mNumFaces * 3);

        const auto vertices = std::span(mesh->mVertices, mesh->mNumVertices);
        const auto normals = std::span(mesh->mNormals, mesh->mNumVertices);
        const auto colors = std::span(mesh->mColors[0], mesh->mNumVertices);
        const auto faces = std::span(mesh->mFaces, mesh->mNumFaces);

        for (size_t i = 0; i < vertices.size(); i++)
        {
            Vertex vertex = {};
            vertex.position = {vertices[i].x, vertices[i].y, vertices[i].z};

            if (mesh->HasNormals())
            {
                vertex.normal = {normals[i].x, normals[i].y, normals[i].z};
            }

            vertex.color = {1.0F, 0.0F, 0.0F};

            vertexBuffer.emplace_back(vertex);
        }

        for (const auto &face : faces)
        {
            const auto indices = std::span(face.mIndices, face.mNumIndices);

            for (const auto index : indices)
            {
                indexBuffer.emplace_back(static_cast<uint32_t>(index));
            }
        }

        const auto &vertex = vertexBuffer[0];
        std::cout << fmt::format("{} {} {}", vertex.position[0], vertex.position[1], vertex.position[2]) << std::endl;
        std::cout << fmt::format("{} {} {}\n", indexBuffer[0], indexBuffer[1], indexBuffer[2]);
        const auto vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
        const auto indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
        const auto totalSize = vertexBufferSize + indexBufferSize;

        std::vector<char> meshBuffer;
        meshBuffer.resize(totalSize);

        memcpy(meshBuffer.data(), vertexBuffer.data(), vertexBufferSize);
        memcpy(meshBuffer.data() + vertexBufferSize, indexBuffer.data(), indexBufferSize);

        auto compressedBound = LZ4_compressBound(totalSize);
        std::vector<char> binaryBlob;
        binaryBlob.resize(compressedBound);
        int compressedSize = LZ4_compress_default(meshBuffer.data(),
                                                  binaryBlob.data(),
                                                  static_cast<int>(meshBuffer.size()),
                                                  static_cast<int>(compressedBound));
        binaryBlob.resize(compressedSize);

        nlohmann::json metadata;
        metadata["numberOfVertex"] = mesh->mNumVertices;
        metadata["numberOfIndex"] = indexBuffer.size();
        metadata["vertexSizeOriginal"] = vertexBufferSize;
        metadata["indexSizeOriginal"] = indexBufferSize;
        metadata["compressedSize"] = compressedSize;
        auto metadataContent = metadata.dump();

        std::ofstream binaryFile;
        binaryFile.open(fmt::format("{}/{}.pvm", "/Users/christian", mesh->mName.C_Str()), std::ios::binary | std::ios::out);

        auto metadataSize = static_cast<uint32_t>(metadataContent.size());
        binaryFile.write(reinterpret_cast<const char *>(&metadataSize), sizeof(uint32_t));
        binaryFile.write(metadataContent.data(), metadataSize);
        binaryFile.write(binaryBlob.data(), binaryBlob.size());

        binaryFile.close();
    }
}
} // namespace

int main(int argc, char *argv[])
{
    // if (argc < 2) {
    //     std::cout << "No path to asset was provided.\n";

    //     return -1;
    // }

    std::filesystem::path filePath{"/Users/christian/walk.glb"};

    std::cout << fmt::format("Loading file {}", filePath.string()) << "\n";

    Assimp::Importer importer;
    const auto *const scene = importer.ReadFile(filePath.string(), assimpFlags);
    loadMeshes(scene);
    // writeJsonToBinary();
    loadJson();
}