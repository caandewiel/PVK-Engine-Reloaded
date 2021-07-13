#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <stdexcept>
#include <string>

#include <absl/container/flat_hash_map.h>

#include <glm/glm.hpp>

#include "../engine/shader/UniformBuffer.hpp"

namespace pvk::geometry
{
constexpr auto MATERIAL_PROPERTY_NAME = "$mat.name";
constexpr auto MATERIAL_COLOR_DIFFUSE = "$clr.diffuse";
constexpr auto MATERIAL_COLOR_AMBIENT = "$clr.ambient";
constexpr auto MATERIAL_COLOR_SPECULAR = "$clr.specular";
constexpr auto MATERIAL_COLOR_EMISSIVE = "$clr.emissive";
constexpr auto MATERIAL_COLOR_TRANSPARENT = "$clr.transparent";
constexpr auto MATERIAL_COLOR_BASE = "$clr.base";

class Material
{
public:
    Material(std::string &&name, absl::flat_hash_map<std::string, std::string> &&customProperties);

    [[nodiscard]] const engine::UniformBuffer& getUniformBuffer() const;

    template <typename T> T getCustomProperty(const std::string &identifier) const
    {
        const auto customProperty = m_customProperties.find(identifier);
        if (customProperty == m_customProperties.end())
        {
            throw std::runtime_error("No material found.");
        }

        const auto value = reinterpret_cast<const T *>(customProperty->second.data());

        return *value;
    }

private:
    std::string m_name;
    absl::flat_hash_map<std::string, std::string> m_customProperties;
    struct
    {
        glm::vec4 colorDiffuse;
    } m_materialRaw;
    std::unique_ptr<engine::UniformBuffer> m_uniformBuffer;
};
} // namespace pvk::geometry

#endif // __MATERIAL_H__