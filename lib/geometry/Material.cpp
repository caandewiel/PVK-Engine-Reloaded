#include "Material.hpp"

#include <memory>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace pvk::geometry
{
Material::Material(std::string &&name,
                   absl::flat_hash_map<std::string, std::string> &&customProperties,
                   absl::flat_hash_map<std::string, std::shared_ptr<engine::Texture>> &&textureLookup)
    : m_name(std::move(name)), m_customProperties(std::move(customProperties)),
      m_textureLookup(std::move(textureLookup))
{
    m_materialRaw.albedo = getCustomProperty<glm::vec3>(MATERIAL_COLOR_DIFFUSE);
    // m_materialRaw.metallicness = getCustomProperty<float>("$clr.specular");
    // m_materialRaw.roughness = getCustomProperty<float>("$mat.shininess");
    m_materialRaw.metallicness = 1.0f;
    m_materialRaw.roughness = 0.2f;
    
    m_uniformBuffer = std::make_unique<pvk::engine::UniformBuffer>(sizeof(m_materialRaw));
    m_uniformBuffer->update(&m_materialRaw);
}

const engine::UniformBuffer &Material::getUniformBuffer() const
{
    return *m_uniformBuffer;
}

const vk::DescriptorSet &Material::getDescriptorSet() const
{
    return m_descriptorSet;
}

std::vector<vk::WriteDescriptorSet> Material::getAllWriteDescriptorSets() const
{
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets = {
        m_uniformBuffer->createWriteDescriptorSet(),
        m_textureLookup.at("BASE_COLOR")->createWriteDescriptorSet(),
        m_textureLookup.at("NORMAL")->createWriteDescriptorSet(),
        m_textureLookup.at("METALLIC_ROUGHNESS")->createWriteDescriptorSet(),
    };

    writeDescriptorSets[0].setDstBinding(0);
    writeDescriptorSets[1].setDstBinding(1);
    writeDescriptorSets[2].setDstBinding(2);
    writeDescriptorSets[3].setDstBinding(3);
    writeDescriptorSets[0].setDstSet(m_descriptorSet);
    writeDescriptorSets[1].setDstSet(m_descriptorSet);
    writeDescriptorSets[2].setDstSet(m_descriptorSet);
    writeDescriptorSets[3].setDstSet(m_descriptorSet);

    return writeDescriptorSets;
}

const engine::Texture &Material::getTexture(const std::string &textureIndex) const
{
    return *m_textureLookup.at(textureIndex);
}
} // namespace pvk::geometry