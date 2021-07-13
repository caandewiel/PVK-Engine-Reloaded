#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <array>
#include <cstddef>
#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace pvk::geometry
{
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec3 color{};
    glm::vec2 uv{};
};

namespace vertex
{
constexpr static std::array<vk::VertexInputBindingDescription, 1> bindingDescriptions = {
    {{0, sizeof(Vertex), vk::VertexInputRate::eVertex}}};

constexpr static std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions = {{
    {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
    {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
    {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
    {3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
}};
} // namespace vertex
} // namespace pvk::geometry

#endif // VERTEX_HPP
