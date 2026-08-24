#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stdalign.h>

namespace Engine {

// // Mesh data
struct alignas(16) GPUMeshEntry {
  public:
    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t indexCount;
    uint32_t padding;
};

// Vertex for the rasterizer
struct alignas(16) GPUVertex {
  public:
    glm::vec4 position;  // w unused
    glm::vec4 normal;    // w unused
    glm::vec4 texCoords; // z, w unused
};

// Triangle instance values
struct alignas(16) GPUInstance {
  public:
    glm::mat4 transform;
    glm::mat4 invTransform;
    uint32_t meshIndex;
    uint32_t materialIndex;
    uint32_t padding[2]; // Pad to 16 bytes
};

// Material values
struct alignas(16) GPUMaterial {
  public:
    glm::vec4 albedo;
    glm::vec4 emmissive;
    float roughness;
    float metallic;
    float padding[2];
};

struct alignas(16) GPUMesh {
  public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    uint32_t indexCount;
};

struct alignas(16) GPUTexture {
  public:
    GLuint textureID;
};

// 1. Global Scene Data (Per Frame)
struct alignas(16) GlobalSceneData {
  public:
    glm::mat4 shadowMatrices[6]; // 6 * 64 = 384 bytes
    glm::vec4 lightPos;          // 16 bytes (xyz = pos, w = unused/padding)
    glm::vec2 resolution;        // 8 bytes
    float fov;                   // 4 bytes
    float shadowFarPlane;        // 4 bytes
};

// 2. Object & Material Data (Per Draw Call)
struct alignas(16) ObjectRenderData {
  public:
    glm::mat4 model;    // 64 bytes
    glm::vec4 albedo;   // 16 bytes (xyz = albedo, w = unused)
    glm::vec4 emissive; // 16 bytes (xyz = emissive, w = unused)
    float roughness;    // 4 bytes
    float metallic;     // 4 bytes
    int isBumpMap;      // 4 bytes
    float padding;      // 4 bytes (explicit padding to hit 16-byte boundary)
};

} // namespace Engine
