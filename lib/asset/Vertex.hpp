#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

namespace pvk::asset
{
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec3 color{};
};
} // namespace pvk::asset

#endif // VERTEX_HPP
