#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <string>
#include <vector>

namespace Engine {

using AssetHandle = uint64_t;
const AssetHandle INVALID_ASSET_HANDLE = 0;

// Vertex information in form for cpu workins
struct CPUVertex {
  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    // Equality opporator for hashing; Checks every attribute
    inline bool operator==(const CPUVertex &other) const {
        return position == other.position && normal == other.normal &&
               texCoords == other.texCoords;
    }
};

// Mesh information for cpu uses
struct CPUMeshData {
    std::string name;

    std::vector<CPUVertex> vertices;
    std::vector<uint32_t> indices;

    // Replaces std::string materialName
    AssetHandle materialHandle = INVALID_ASSET_HANDLE;
};

// Model information; stores a list of CPUMeshDatas
struct CPUModelData {
    inline CPUModelData(std::vector<AssetHandle> _meshHandles)
        : meshHandles(_meshHandles) {}
    std::vector<AssetHandle> meshHandles;
};

struct CPUTextureData {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 0;

    // TODO: own format enum

    std::vector<unsigned char> pixels;
};

// Material information for use on the cpu
struct CPUMaterialData {
    std::string name;

    glm::vec3 albedo = glm::vec3(1.0f);
    glm::vec3 emissive = glm::vec3(0.0f);
    float roughness = 1.0;
    float metallic = 0.0;

    std::unordered_map<std::string, AssetHandle> textureMaps;
};

} // namespace Engine

namespace std {

// Hash function for the vertex
template <> struct hash<Engine::CPUVertex> {
  public:
    inline size_t operator()(const Engine::CPUVertex &vertex) const {
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
