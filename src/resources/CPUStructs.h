#pragma once

#include <glm/glm.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <string>

namespace Engine {
struct CPUVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    // Equality opporator, used for hashing
    bool operator==(const CPUVertex &other) const {
        return position == other.position && normal == other.normal &&
               texCoords == other.texCoords;
    }
};

// Mesh information
struct CPUMeshData {
    std::string name;
    std::vector<CPUVertex> vertices;
    std::vector<uint32_t> indices;
};

struct CPUMaterialData {
    std::string name;

    glm::vec3 albedo{1.0f, 0.0f, 1.0f};
    glm::vec3 emmission{0.0f, 0.0f, 0.0f};

    float roughness = 1.0f;
    float metallic = 0.0f;
    float ior = 0.0f;
};

struct CPURenderableEntityData {
    CPUMeshData meshData;
    std::vector<CPUMaterialData> materials;
};
} // namespace Engine

namespace std {
template <> struct hash<Engine::CPUVertex> {
    size_t operator()(Engine::CPUVertex const &vertex) const {
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
