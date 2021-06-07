#include "Blueprint.hpp"

namespace pvk::asset
{
bool NodeInfo::hasMesh() const
{
    return !meshIndices.empty();
}

nlohmann::json NodeInfo::toJson() const
{
    return {
        {"identifier", identifier},
        {"name", name},
        {"parent", parent},
        {"children", children},
        {"meshIndices", meshIndices},
    };
}

NodeInfo NodeInfo::parseJson(const nlohmann::json &json)
{
    NodeInfo node{};
    node.identifier = json["identifier"];
    node.name = json["name"];
    node.parent = json["parent"];
    node.children = json["children"].get<std::vector<uint32_t>>();
    node.meshIndices = json["meshIndices"].get<std::vector<uint32_t>>();

    return node;
}

nlohmann::json Blueprint::toJson() const
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

Blueprint Blueprint::parseJson(const nlohmann::json &json)
{
    Blueprint blueprint{};

    for (const auto &node : json["nodes"])
    {
        blueprint.nodes.emplace_back(NodeInfo::parseJson(node));
    }

    blueprint.meshPaths = json["meshPaths"].get<std::vector<std::filesystem::path>>();

    return blueprint;
}
} // namespace pvk::asset
