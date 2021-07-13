#include "MaterialInfo.hpp"

namespace pvk::asset
{

nlohmann::json MaterialInfo::toJson() const
{
    return {
        {"identifier", identifier},
        {"name", name},
        {"customProperties", customData},
        {"textureData", textureData},
    };
}

MaterialInfo MaterialInfo::parseJson(const nlohmann::json &json)
{
    MaterialInfo materialInfo;

    materialInfo.identifier = json["identifier"];
    materialInfo.name = json["name"];
    materialInfo.customData = json["customProperties"];
    materialInfo.textureData = json["textureData"];

    return materialInfo;
}

} // namespace pvk::asset
