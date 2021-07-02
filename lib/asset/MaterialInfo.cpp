#include "MaterialInfo.hpp"

namespace pvk::asset
{

nlohmann::json MaterialInfo::toJson() const
{
    return {
        {"identifier", identifier},
        {"name", name},
        {"customProperties", customData},
    };
}

MaterialInfo MaterialInfo::parseJson(const nlohmann::json &json)
{
    MaterialInfo materialInfo;

    materialInfo.identifier = json["identifier"];
    materialInfo.name = json["name"];
    materialInfo.customData = json["customProperties"];

    return materialInfo;
}

} // namespace pvk::asset
