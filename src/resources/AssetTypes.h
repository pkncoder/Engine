#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace Engine {

// Vertex w/ attributes
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// Mesh information
struct MeshData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

} // namespace Engine
