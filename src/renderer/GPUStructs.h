#pragma once

#include <glm/glm.hpp>

namespace Engine {

struct alignas(16) GPUMeshEntry {
    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t indexCount;
    uint32_t padding;
};

struct alignas(16) GPUVertex {
    glm::vec4 position;  // w unused
    glm::vec4 normal;    // w unused
    glm::vec4 texCoords; // z, w unused
};

struct alignas(16) GPUInstance {
    glm::mat4 transform;
    glm::mat4 invTransform;
    uint32_t meshIndex;
    uint32_t padding[3]; // Pad to 16 bytes
};

} // namespace Engine
