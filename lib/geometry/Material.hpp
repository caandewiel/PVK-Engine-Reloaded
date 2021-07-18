#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <memory>
#include <stdexcept>
#include <string>

#include <absl/container/flat_hash_map.h>

#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>

#include "../engine/render_stage/Shader.hpp"
#include "../engine/shader/Texture.hpp"
#include "../engine/shader/UniformBuffer.hpp"

namespace pvk::geometry
{
class Object;

constexpr auto MATERIAL_PROPERTY_NAME = "$mat.name";
constexpr auto MATERIAL_COLOR_DIFFUSE = "$clr.diffuse";
constexpr auto MATERIAL_COLOR_AMBIENT = "$clr.ambient";
constexpr auto MATERIAL_COLOR_SPECULAR = "$clr.specular";
constexpr auto MATERIAL_COLOR_EMISSIVE = "$clr.emissive";
constexpr auto MATERIAL_COLOR_TRANSPARENT = "$clr.transparent";
constexpr auto MATERIAL_COLOR_BASE = "$clr.base";
constexpr auto MATERIAL_PBR_METALLIC_FACTOR = "$mat.metallicFactor";
constexpr auto MATERIAL_PBR_ROUGHNESS_FACTOR = "$mat.roughnessFactor";

class Material : public std::enable_shared_from_this<Material>
{
public:
    Material(std::string &&name,
             absl::flat_hash_map<std::string, std::string> &&customProperties,
             absl::flat_hash_map<std::string, std::shared_ptr<engine::Texture>> &&textureLookup);

    [[nodiscard]] const engine::UniformBuffer &getUniformBuffer() const;
    [[nodiscard]] const vk::DescriptorSet &getDescriptorSet() const;
    [[nodiscard]] std::vector<vk::WriteDescriptorSet> getAllWriteDescriptorSets() const;

    template <typename T> T getCustomProperty(const std::string &identifier) const
    {
        // for (auto &[a, _] : m_customProperties) {
        //     std::cout << a << "\n";
        // }
        const auto customProperty = m_customProperties.find(identifier);
        if (customProperty == m_customProperties.end())
        {
            throw std::runtime_error("No material found.");
        }

        const auto value = reinterpret_cast<const T *>(customProperty->second.data());

        return *value;
    }

    [[nodiscard]] const engine::Texture &getTexture(const std::string &textureIndex) const;

    friend void engine::Shader::bindObjectDescriptor(
        const geometry::Object &object,
        const std::string &identifier,
        const std::function<const engine::Descriptor &(const geometry::Object &object, const geometry::Mesh &mesh)>
            &callback);

private:
    std::string m_name;
    absl::flat_hash_map<std::string, std::string> m_customProperties;
    absl::flat_hash_map<std::string, std::shared_ptr<engine::Texture>> m_textureLookup;
    struct
    {
        glm::vec3 albedo;
        float metallicness;
        float roughness;
    } m_materialRaw;
    std::unique_ptr<engine::UniformBuffer> m_uniformBuffer;

    vk::DescriptorSet m_descriptorSet;
};
} // namespace pvk::geometry

#endif // __MATERIAL_H__