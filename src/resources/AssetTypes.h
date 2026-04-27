#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Engine {

// A single vertex containing all necessary attributes
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// The raw data of a 3D model
struct MeshData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

} // namespace Engine
