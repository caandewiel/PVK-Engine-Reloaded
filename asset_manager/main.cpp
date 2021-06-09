// Insipred by: https://vkguide.dev/docs/extra-chapter/asset_system/

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <span>
#include <streambuf>
#include <utility>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <fmt/format.h>

#include <lz4.h>

#include <json.hpp>

#include "../lib/asset/AssetHelper.hpp"
#include "../lib/asset/Blueprint.hpp"

constexpr auto assimpFlags = aiProcess_OptimizeMeshes | aiProcess_GenNormals | aiProcess_FlipUVs;
constexpr auto meshFlag = "MESH";
// constexpr auto textureFlag = "TEX ";

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec3 color{};
};

struct Object
{
    absl::flat_hash_map<uint32_t, pvk::asset::NodeInfo> nodeLookup;
    std::vector<glm::mat4> matrices;
};

namespace
{
// void loadJson()
// {
//     {
//         std::ifstream inputFile;
//         inputFile.open(fmt::format("{}/{}.object", "/Users/christian", "walk/walk"), std::ios::binary);
//         inputFile.seekg(0);

//         uint32_t metadataSize = 0;
//         inputFile.read(reinterpret_cast<char *>(&metadataSize), sizeof(uint32_t));

//         std::string metadataContent;
//         metadataContent.resize(metadataSize);
//         inputFile.read(metadataContent.data(), metadataSize);

//         auto metadata = nlohmann::json::parse(metadataContent.begin(), metadataContent.end());

//         uint32_t matricesSize = 0;
//         inputFile.read(reinterpret_cast<char *>(&matricesSize), sizeof(uint32_t));

//         std::vector<char> matricesBinaryBlob;
//         matricesBinaryBlob.resize(matricesSize);
//         inputFile.read(matricesBinaryBlob.data(), matricesSize);

//         auto blueprint = pvk::asset::Blueprint::parseJson(metadata);
//         blueprint.matrices = pvk::binary::convertBinaryBlobToVector<glm::mat4>(std::move(matricesBinaryBlob));

//         std::cout << metadata.dump() << std::endl;
//     }

//     std::ifstream inputFile;
//     inputFile.open(fmt::format("{}/{}.mesh", "/Users/christian", "walk/Beta_Surface"), std::ios::binary);
//     inputFile.seekg(0);

//     std::string flags;
//     flags.resize(4);
//     inputFile.read(flags.data(), 4 * sizeof(char));

//     std::cout << flags.c_str() << "\n";

//     uint32_t metadataSize = 0;
//     inputFile.read(reinterpret_cast<char *>(&metadataSize), sizeof(uint32_t));

//     std::string metadataContent;
//     metadataContent.resize(metadataSize);
//     inputFile.read(metadataContent.data(), metadataSize);

//     auto metadata = nlohmann::json::parse(metadataContent.begin(), metadataContent.end());

//     const auto vertexSizeOriginal = metadata["vertexSizeOriginal"].get<uint32_t>();
//     const auto indexSizeOriginal = metadata["indexSizeOriginal"].get<uint32_t>();
//     const auto targetSize = vertexSizeOriginal + indexSizeOriginal;
//     const auto compressedSize = metadata["compressedSize"].get<uint32_t>();

//     std::string binaryBlob;
//     binaryBlob.resize(targetSize);
//     inputFile.read(binaryBlob.data(), compressedSize);

//     std::string output;
//     output.resize(targetSize);

//     LZ4_decompress_safe(binaryBlob.data(), output.data(), compressedSize, targetSize);

//     std::vector<char> vertexBuffer;
//     std::vector<char> indexBuffer;
//     vertexBuffer.resize(vertexSizeOriginal);
//     indexBuffer.resize(indexSizeOriginal);
//     memcpy(vertexBuffer.data(), output.data(), vertexSizeOriginal);
//     memcpy(indexBuffer.data(), output.data() + vertexSizeOriginal, indexSizeOriginal);
//     auto vertices = std::span(reinterpret_cast<Vertex *>(vertexBuffer.data()), vertexSizeOriginal / sizeof(Vertex));
//     auto indices = std::span(reinterpret_cast<uint32_t *>(indexBuffer.data()), indexSizeOriginal / sizeof(uint32_t));

//     std::cout << vertices[0].position.x << "\n";
// }

void loadMeshes(const aiScene *scene, const std::filesystem::path &destinationPath)
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

            if (mesh->HasVertexColors(0))
            {
                vertex.color = {colors[i].r, colors[i].g, colors[i].b};
            }

            vertexBuffer.emplace_back(vertex);
        }

        for (const auto &face : faces)
        {
            const auto indices = std::span(face.mIndices, face.mNumIndices);
            indexBuffer.insert(indexBuffer.end(), indices.begin(), indices.end());
        }

        const auto vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
        const auto indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
        const auto totalSize = vertexBufferSize + indexBufferSize;

        std::vector<char> meshBuffer;
        meshBuffer.resize(totalSize);

        memcpy(meshBuffer.data(), vertexBuffer.data(), vertexBufferSize);
        memcpy(meshBuffer.data() + vertexBufferSize, indexBuffer.data(), indexBufferSize);

        auto binaryBlob = pvk::asset::compress(std::move(meshBuffer));

        nlohmann::json metadata;
        metadata["numberOfVertex"] = mesh->mNumVertices;
        metadata["numberOfIndex"] = indexBuffer.size();
        metadata["vertexSizeOriginal"] = vertexBufferSize;
        metadata["indexSizeOriginal"] = indexBufferSize;
        metadata["compressedSize"] = binaryBlob.size();
        auto metadataContent = metadata.dump();

        std::ofstream binaryFile;
        const auto destinationPathMesh = (destinationPath / mesh->mName.C_Str()).replace_extension("mesh");
        binaryFile.open(destinationPathMesh, std::ios::binary | std::ios::out);

        std::cout << "Written mesh to " << destinationPathMesh << "\n";

        auto metadataSize = static_cast<uint32_t>(metadataContent.size());
        binaryFile.write(meshFlag, 4 * sizeof(char));
        binaryFile.write(reinterpret_cast<const char *>(&metadataSize), sizeof(uint32_t));
        binaryFile.write(metadataContent.data(), metadataSize);
        binaryFile.write(binaryBlob.data(), binaryBlob.size());

        binaryFile.close();
    }
}

void loadNode(const aiScene *scene,
              const aiNode *node,
              const std::shared_ptr<pvk::asset::Blueprint> &blueprint,
              int32_t parentIndex = -1)
{
    const auto currentIndex = blueprint->matrices.size();

    pvk::asset::NodeInfo currentNode;
    currentNode.identifier = currentIndex;
    currentNode.name = node->mName.C_Str();
    currentNode.parent = parentIndex;

    glm::mat4 modelMatrix{};

    for (size_t y = 0; y < 4; y++)
    {
        for (size_t x = 0; x < 4; x++)
        {
            modelMatrix[y][x] = node->mTransformation[x][y];
        }
    }

    for (const auto &mesh : std::span(node->mMeshes, node->mNumMeshes)) {
        currentNode.meshIndices.emplace_back(mesh);
    }

    blueprint->nodes.emplace_back(std::move(currentNode));
    blueprint->matrices.emplace_back(modelMatrix);

    for (const auto &child : std::span(node->mChildren, node->mNumChildren))
    {
        loadNode(scene, child, blueprint, currentIndex);
    }
}

void loadNodes(const aiScene *scene, const std::filesystem::path &destinationPath, const std::filesystem::path &stem)
{
    auto blueprint = std::make_shared<pvk::asset::Blueprint>();
    loadNode(scene, scene->mRootNode, blueprint);

    const auto meshes = std::span(scene->mMeshes, scene->mNumMeshes);
    for (const auto &mesh : meshes) {
        blueprint->meshPaths.emplace_back(std::filesystem::path(mesh->mName.C_Str()).replace_extension(".mesh"));
    }

    const auto jsonContent = blueprint->toJson().dump();
    const uint32_t jsonContentSize = jsonContent.size();

    const auto binaryBlobMatrices = pvk::asset::convertVectorToBinary(std::move(blueprint->matrices));
    const uint32_t binaryBlobMatricesSize = binaryBlobMatrices.size();

    const auto destinationPathBlueprint = (destinationPath / stem).replace_extension("object");

    std::ofstream binaryFile;
    binaryFile.open(destinationPathBlueprint);
    binaryFile.write(reinterpret_cast<const char *>(&jsonContentSize), sizeof(uint32_t));
    binaryFile.write(jsonContent.data(), jsonContent.size());
    binaryFile.write(reinterpret_cast<const char *>(&binaryBlobMatricesSize), sizeof(uint32_t));
    binaryFile.write(binaryBlobMatrices.data(), binaryBlobMatrices.size());
    binaryFile.close();

    std::cout << "Written blueprint to " << destinationPathBlueprint << "\n";
}
} // namespace

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "No path to asset was provided.\n";

        return -1;
    }

    std::filesystem::path filePath{argv[1]};
// std::filesystem::path filePath{"/Users/christian/walk.glb"};

    std::cout << fmt::format("Loading file {}", filePath.string()) << "\n";

    Assimp::Importer importer;
    const auto *const scene = importer.ReadFile(filePath.string(), assimpFlags);

    const auto destinationPath = filePath.parent_path() / filePath.stem();

    std::cout << "Exporting converted files to " << destinationPath << "\n";
    std::filesystem::create_directory(destinationPath);

    loadMeshes(scene, destinationPath);
    loadNodes(scene, destinationPath, filePath.stem());
    // writeJsonToBinary();
    // loadJson();
}