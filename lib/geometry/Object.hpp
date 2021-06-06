#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <absl/container/flat_hash_map.h>
#include <filesystem>
#include <memory>

#include "Mesh.hpp"
#include "Node.hpp"

namespace pvk::geometry
{
class Object
{
public:
    Object(absl::flat_hash_map<uint32_t, std::unique_ptr<Mesh>> &&meshLookup,
           absl::flat_hash_map<uint32_t, std::shared_ptr<Node>> &&nodeLookup);

    [[nodiscard]] const Mesh &getMesh(uint32_t meshIndex) const;
    [[nodiscard]] const Node &getNode(uint32_t nodeIndex) const;
    [[nodiscard]] const Node &getRootNode() const;

private:
    absl::flat_hash_map<uint32_t, std::unique_ptr<Mesh>> m_meshLookup{};
    absl::flat_hash_map<uint32_t, std::shared_ptr<Node>> m_nodeLookup{};
};
} // namespace pvk::geometry

#endif // OBJECT_HPP
