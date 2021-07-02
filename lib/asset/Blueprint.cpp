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
        {"materialIndices", materialIndices},
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
    node.materialIndices = json["materialIndices"].get<std::vector<uint32_t>>();

    return node;
}

nlohmann::json Blueprint::toJson() const
{
    auto json = nlohmann::json();
    std::vector<nlohmann::json> jsonNodes;
    std::vector<nlohmann::json> jsonMaterials;

    for (const auto &node : nodes)
    {
        jsonNodes.emplace_back(node.toJson());
    }

    for (const auto &material : materials)
    {
        jsonMaterials.emplace_back(material.toJson());
    }

    json["nodes"] = jsonNodes;
    json["materials"] = jsonMaterials;
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

    for (const auto &material : json["materials"])
    {
        blueprint.materials.emplace_back(MaterialInfo::parseJson(material));
    }

    blueprint.meshPaths = json["meshPaths"].get<std::vector<std::filesystem::path>>();

    return blueprint;
}
} // namespace pvk::asset
