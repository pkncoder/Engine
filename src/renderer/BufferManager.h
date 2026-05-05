#pragma once

#include "../resources/AssetTypes.h"
#include "../scene/components/MeshComponent.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef> // for size_t

namespace Engine {

// A modern, persistently mapped GPU buffer
struct PersistentBuffer {
    GLuint id = 0;
    void *mappedPtr = nullptr;
    size_t size = 0;
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
    static MeshComponent uploadMesh(const MeshData &meshData);
};

} // namespace Engine
