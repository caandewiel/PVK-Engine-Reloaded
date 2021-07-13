#include "Material.hpp"

#include <memory>
#include <utility>

namespace pvk::geometry
{
Material::Material(std::string &&name, absl::flat_hash_map<std::string, std::string> &&customProperties)
    : m_name(std::move(name)), m_customProperties(std::move(customProperties))
{
    m_materialRaw.colorDiffuse = getCustomProperty<glm::vec4>(MATERIAL_COLOR_DIFFUSE);
    m_uniformBuffer = std::make_unique<pvk::engine::UniformBuffer>(sizeof(m_materialRaw));
    m_uniformBuffer->update(&m_materialRaw);
}

const engine::UniformBuffer& Material::getUniformBuffer() const
{
    return *m_uniformBuffer;
}
} // namespace pvk::geometry