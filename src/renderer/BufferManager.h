#pragma once

#include "../resources/CPUStructs.h"
#include "../scene/components/MeshComponent.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef> // for size_t

namespace Engine {

// A modern, persistently mapped GPU buffer
// TODO: format
struct PersistentBuffer {
    GLuint id = 0;
    GLuint bindingIndex = 0;

    void *mappedPtr = nullptr;
    size_t size = 0;
    size_t elementSize = 0;
    GLenum target = GL_SHADER_STORAGE_BUFFER;

    // Allocates immutable VRAM and maps it permanently to CPU space
    void setup(GLenum bufferTarget, size_t bufferSize);

    // Fast copy data directly to VRAM
    void update(const void *data, size_t updateSize);

    void shutdown();
};

class BufferManager {
  public:
    // Keep your existing Rasterizer methods here...
    static MeshComponent uploadMesh(const CPUMeshData &meshData);

  private:
    static inline std::unordered_map<std::string, MeshComponent> gpuMeshCache;
};

} // namespace Engine
