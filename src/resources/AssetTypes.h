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

    // Equality opporator, used for hashing
    bool operator==(const Vertex &other) const {
        return position == other.position && normal == other.normal &&
               texCoords == other.texCoords;
    }
};

// Mesh information
struct MeshData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

} // namespace Engine
