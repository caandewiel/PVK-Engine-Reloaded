#ifndef PREFAB_HPP
#define PREFAB_HPP

#include <filesystem>
#include <string>
#include <unordered_map>

#include <absl/container/flat_hash_map.h>

#include <glm/glm.hpp>

#include <json.hpp>
#include <vector>

namespace pvk::asset
{
struct NodeInfo
{
    [[nodiscard]] bool hasMesh() const;

    [[nodiscard]] nlohmann::json toJson() const;

    static NodeInfo parseJson(const nlohmann::json &json);

    uint32_t identifier;
    std::string name;
    int32_t parent; // -1 when no parent
    std::vector<uint32_t> meshIndices;
};

struct Blueprint
{
    [[nodiscard]] nlohmann::json toJson() const;

    static Blueprint parseJson(const nlohmann::json &json);

    std::vector<NodeInfo> nodes;
    std::vector<glm::mat4> matrices;
    std::vector<std::filesystem::path> meshPaths;
};
} // namespace pvk::asset

#endif // PREFAB_HPP
