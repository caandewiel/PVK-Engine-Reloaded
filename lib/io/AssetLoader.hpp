#ifndef __ASSETLOADER_H__
#define __ASSETLOADER_H__

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "../geometry/Vertex.hpp"
#include "../geometry/Object.hpp"

#include "../engine/shader/Texture.hpp"

namespace pvk::io
{
std::pair<std::vector<geometry::Vertex>, std::vector<uint32_t>> loadMeshBuffers(const std::filesystem::path &path);
std::shared_ptr<engine::Texture> loadTexture(const std::filesystem::path &path);
std::shared_ptr<geometry::Object> loadObject(const std::filesystem::path &path);
}

#endif // __ASSETLOADER_H__