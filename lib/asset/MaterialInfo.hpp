#ifndef __MATERIALINFO_H__
#define __MATERIALINFO_H__

#include <filesystem>
#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include <json.hpp>

namespace pvk::asset
{
struct MaterialInfo
{
    [[nodiscard]] nlohmann::json toJson() const;

    static MaterialInfo parseJson(const nlohmann::json &json);

    uint32_t identifier;
    std::string name;
    absl::flat_hash_map<std::string, std::string> customData;
    absl::flat_hash_map<std::string, std::filesystem::path> textureData;
};
} // namespace pvk::asset

#endif // __MATERIALINFO_H__