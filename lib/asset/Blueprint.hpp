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
    uint32_t identifier;
    std::string name;
    int32_t parent; // -1 when no parent
    std::vector<uint32_t> meshIndices;

    [[nodiscard]] bool hasMesh() const
    {
        return !meshIndices.empty();
    }

    [[nodiscard]] nlohmann::json toJson() const
    {
        return {
            {"identifier", identifier},
            {"name", name},
            {"parent", parent},
            {"meshIndices", meshIndices},
        };
    }

    static NodeInfo parseJson(const nlohmann::json &json)
    {
        NodeInfo node{};
        node.identifier = json["identifier"];
        node.name = json["name"];
        node.parent = json["parent"];
        node.meshIndices = json["meshIndices"].get<std::vector<uint32_t>>();

        return node;
    }
};

struct Blueprint
{
    [[nodiscard]] nlohmann::json toJson() const
    {
        auto json = nlohmann::json();
        std::vector<nlohmann::json> jsonNodes;

        for (const auto &node : nodes)
        {
            jsonNodes.emplace_back(node.toJson());
        }

        json["nodes"] = jsonNodes;
        json["meshPaths"] = meshPaths;

        return json;
    }

    static Blueprint parseJson(const nlohmann::json &json)
    {
        Blueprint blueprint{};

        for (const auto &node : json["nodes"]) {
            blueprint.nodes.emplace_back(NodeInfo::parseJson(node));
        }

        blueprint.meshPaths = json["meshPaths"].get<std::vector<std::filesystem::path>>();

        return blueprint;
    }

    std::vector<NodeInfo> nodes;
    std::vector<glm::mat4> matrices;
    std::vector<std::filesystem::path> meshPaths;
};
} // namespace pvk::asset

#endif // PREFAB_HPP
