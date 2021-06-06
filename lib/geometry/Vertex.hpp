#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "glm/fwd.hpp"
#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace pvk::geometry
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

namespace vertex
{
constexpr static std::array<vk::VertexInputBindingDescription, 1> bindingDescriptions = {
    {{0, sizeof(Vertex), vk::VertexInputRate::eVertex}}};

constexpr static std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {{
    {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
    {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
    {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
}};
} // namespace vertex
} // namespace pvk::geometry

#endif // VERTEX_HPP
