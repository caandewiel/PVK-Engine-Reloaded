#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <filesystem>
#include <memory>
#include <string>

#include <absl/container/flat_hash_map.h>

#include "Drawable.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Node.hpp"
#include "../engine/shader/Texture.hpp"

namespace pvk::geometry
{
class Object : public Drawable
{
public:
    Object(absl::flat_hash_map<uint32_t, std::shared_ptr<Mesh>> &&meshLookup,
           absl::flat_hash_map<uint32_t, std::shared_ptr<Node>> &&nodeLookup,
           absl::flat_hash_map<uint32_t, std::shared_ptr<Material>> &&materialLookup,
           absl::flat_hash_map<std::string, std::shared_ptr<engine::Texture>> &&textureLookup);

    [[nodiscard]] const engine::Texture &getTexture(const std::string &textureIndex) const;

    [[nodiscard]] const Mesh &getMesh(uint32_t meshIndex) const;
    [[nodiscard]] size_t getNumberOfMeshes() const;

    [[nodiscard]] const Node &getNode(uint32_t nodeIndex) const;
    [[nodiscard]] const Node &getRootNode() const;

    void draw(const pvk::command_buffer::CommandBuffer &commandBuffer) const override;
    void pushConstant(const std::string &identifier, void *data);

    [[nodiscard]] const void *getPushConstantData(const std::string &identifier) const;

    [[nodiscard]] std::pair<glm::vec3, glm::vec3> getBounds() const;

private:
    absl::flat_hash_map<uint32_t, std::shared_ptr<Mesh>> m_meshLookup{};
    absl::flat_hash_map<uint32_t, std::shared_ptr<Node>> m_nodeLookup{};
    absl::flat_hash_map<uint32_t, std::shared_ptr<Material>> m_materialLookup{};
    absl::flat_hash_map<std::string, std::shared_ptr<engine::Texture>> m_textureLookup{};
    absl::flat_hash_map<std::string, void *> m_pushConstants{};

    glm::vec3 m_minBounds;
    glm::vec3 m_maxBounds;
};
} // namespace pvk::geometry

#endif // OBJECT_HPP
