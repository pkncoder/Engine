#pragma once

#include <OpenGL/OpenGL.h>
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
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    uint32_t indexCount;
};

struct alignas(16) GPUTexture {
    GLuint textureID;
};

} // namespace Engine
