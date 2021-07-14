// Insipred by: https://vkguide.dev/docs/extra-chapter/asset_system/

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <streambuf>
#include <utility>
#include <vector>
#include <span>

#include <absl/container/flat_hash_map.h>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/texture.h>

#include <fmt/format.h>

#include <gli.hpp>

#include <lz4.h>

#include <json.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "../lib/asset/AssetHelper.hpp"
#include "../lib/asset/Blueprint.hpp"
#include "../lib/asset/MaterialInfo.hpp"

constexpr auto assimpFlags =
    aiProcess_OptimizeMeshes | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_EmbedTextures;
constexpr auto meshFlag = "MESH";
// constexpr auto materialFlag = "MAT_";
// constexpr auto textureFlag = "TEX ";

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec3 color{};
    glm::vec2 uv{};
};

struct Object
{
    absl::flat_hash_map<uint32_t, pvk::asset::NodeInfo> nodeLookup;
    std::vector<glm::mat4> matrices;
};

namespace
{
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
        const auto uvs = std::span(mesh->mTextureCoords[0], mesh->mNumVertices);
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

            if (mesh->HasTextureCoords(0))
            {
                vertex.uv = {uvs[i].x, uvs[i].y};
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

        const auto metadataSize = htonl(metadataContent.size());
        binaryFile.write(meshFlag, 4 * sizeof(char));
        binaryFile.write(reinterpret_cast<const char *>(&metadataSize), sizeof(uint32_t));
        binaryFile.write(metadataContent.data(), metadataContent.size());
        binaryFile.write(binaryBlob.data(), binaryBlob.size());

        binaryFile.close();
    }
}

void loadTextures(const aiScene *scene, const std::filesystem::path &destinationPath)
{
    const auto textures = std::span(scene->mTextures, scene->mNumTextures);

    for (size_t i = 0; i < textures.size(); i++)
    {
        const auto &texture = textures[i];
        std::vector<unsigned char> textureBuffer;

        // If height is 0, take only width
        size_t textureBufferSize;
        const auto textureFormat = std::string(texture->achFormatHint);

        if (texture->mHeight == 0 && (textureFormat == "png" || textureFormat == "jpg"))
        {
            textureBufferSize = static_cast<uint64_t>(texture->mWidth);
            textureBuffer.resize(textureBufferSize);
            memcpy(textureBuffer.data(), texture->pcData, textureBufferSize);

            auto destinationPathTexture =
                destinationPath / std::filesystem::path(std::to_string(i)).replace_extension(textureFormat);

            std::ofstream binaryFile;
            binaryFile.open(destinationPathTexture, std::ios::binary | std::ios::out);
            binaryFile.write(reinterpret_cast<const char *>(textureBuffer.data()), textureBufferSize);

            std::cout << "Written texture to " << destinationPathTexture << "\n";
        }
        else
        {
            throw std::runtime_error("Unsupported texture format");
        }
    }
}

constexpr std::array<std::pair<aiTextureType, const char *>, 2> textureTypes = {
    {{aiTextureType_DIFFUSE, "diffuse"}, {aiTextureType_BASE_COLOR, "baseColor"}}};

void loadMaterials(const aiScene *scene, const std::shared_ptr<pvk::asset::Blueprint> &blueprint)
{
    std::vector<nlohmann::json> materialInfos;
    const auto materials = std::span(scene->mMaterials, scene->mNumMaterials);
    auto currentMaterialIndex = 0;

    const auto textures = std::span(scene->mTextures, scene->mNumTextures);

    for (const auto &material : materials)
    {
        pvk::asset::MaterialInfo materialInfo{};
        materialInfo.identifier = currentMaterialIndex;
        materialInfo.name = material->GetName().C_Str();

        for (const auto &[textureType, textureIndex] : textureTypes)
        {
            aiString texturePath;
            if (material->GetTextureCount(textureType) > 0)
            {
                material->GetTexture(textureType, 0, &texturePath);

                std::filesystem::path destination;

                if (std::filesystem::path(texturePath.C_Str()).string().substr(0, 1) == "*")
                {
                    destination = std::filesystem::path(texturePath.C_Str()).string().substr(1, 2);
                }
                else
                {
                    for (const auto &texture : textures)
                    {
                        std::cout << std::filesystem::path(texture->mFilename.C_Str()).string() << " - "
                                  << std::string(texturePath.C_Str()) << "\n";
                        if (std::filesystem::path(texture->mFilename.C_Str()).string() ==
                            std::string(texturePath.C_Str()))
                        {
                        }

                        // const char *shortTextureFilename = GetShortFilename(mTextures[i]->mFilename.C_Str());
                        // if (strcmp(shortTextureFilename, shortFilename) == 0)
                        // {
                        //     return mTextures[i];
                        // }
                    }
                }
                std::cout << destination.replace_extension(scene->GetEmbeddedTexture(texturePath.C_Str())->achFormatHint) << "\n";

                materialInfo.textureData.insert({textureIndex,
                                                 destination.replace_extension(
                                                     scene->GetEmbeddedTexture(texturePath.C_Str())->achFormatHint)});
            }
        }

        const auto properties = std::span(material->mProperties, material->mNumProperties);

        for (const auto &property : properties)
        {
            materialInfo.customData.insert({property->mKey.C_Str(), property->mData});
        }

        blueprint->materials.emplace_back(std::move(materialInfo));

        currentMaterialIndex++;
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

    const auto &meshes = std::span(scene->mMeshes, scene->mNumMeshes);

    for (const auto &mesh : std::span(node->mMeshes, node->mNumMeshes))
    {
        currentNode.meshIndices.emplace_back(mesh);
        currentNode.materialIndices.emplace_back(meshes[mesh]->mMaterialIndex);
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
    loadMaterials(scene, blueprint);

    const auto meshes = std::span(scene->mMeshes, scene->mNumMeshes);
    for (const auto &mesh : meshes)
    {
        blueprint->meshPaths.emplace_back(std::filesystem::path(mesh->mName.C_Str()).replace_extension(".mesh"));
    }

    const auto blueprintContent = nlohmann::json::to_msgpack(blueprint->toJson());
    const auto blueprintContentSize = htonl(blueprintContent.size());

    const auto binaryBlobMatrices = pvk::asset::convertVectorToBinary(std::move(blueprint->matrices));
    const auto binaryBlobMatricesSize = htonl(binaryBlobMatrices.size());

    const auto destinationPathBlueprint = (destinationPath / stem).replace_extension("object");

    std::ofstream binaryFile(destinationPathBlueprint, std::ios::out | std::ios::binary);
    binaryFile.write(reinterpret_cast<const char *>(&blueprintContentSize), sizeof(uint32_t));
    binaryFile.write(reinterpret_cast<const char *>(blueprintContent.data()), blueprintContent.size());
    binaryFile.write(reinterpret_cast<const char *>(&binaryBlobMatricesSize), sizeof(uint32_t));
    binaryFile.write(binaryBlobMatrices.data(), binaryBlobMatrices.size());
    binaryFile.close();

    std::cout << "Written blueprint to " << destinationPathBlueprint << "\n";
}
} // namespace

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "No path to asset was provided.\n";

        return -1;
    }

    std::filesystem::path filePath{argv[1]};
    // std::filesystem::path filePath{"/Users/christian/Downloads/cyber_warrior/scene.gltf"};

    std::cout << fmt::format("Loading file {}", filePath.string()) << "\n";

    Assimp::Importer importer;
    const auto *const scene = importer.ReadFile(filePath.string(), assimpFlags);

    const auto destinationPath = filePath.parent_path() / filePath.stem();

    std::cout << "Exporting converted files to " << destinationPath << "\n";
    std::filesystem::create_directory(destinationPath);

    loadMeshes(scene, destinationPath);
    loadNodes(scene, destinationPath, filePath.stem());
    loadTextures(scene, destinationPath);
    // writeJsonToBinary();
    // loadJson();
}