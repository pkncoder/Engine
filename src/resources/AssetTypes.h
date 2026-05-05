#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

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

namespace std {
template <> struct hash<Engine::Vertex> {
    size_t operator()(Engine::Vertex const &vertex) const {
        // Using a simple bit-shifting combine method
        size_t res = 0;

        // Seperate the data evenly through the hash
        res ^= hash<glm::vec3>()(vertex.position) + 0x9e3779b9 + (res << 6) +
               (res >> 2);
        res ^= hash<glm::vec3>()(vertex.normal) + 0x9e3779b9 + (res << 6) +
               (res >> 2);
        res ^= hash<glm::vec2>()(vertex.texCoords) + 0x9e3779b9 + (res << 6) +
               (res >> 2);

        // Retrun the final hash
        return res;
    }
};
} // namespace std
