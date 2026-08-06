#pragma once

#include "BufferManager.h"
#include <cstddef>
#include <glad/glad.h>

namespace Engine {

struct SSBO {

  public:
    // Alocate VRAM and cache data to CPU
    void setup(const GLenum bufferTarget, const size_t bufferSize);

    // Update saved data, including in VRAM
    void update(const void *data, const size_t updateSize);

    // Data cleanup
    void shutdown();

  public:
    // ID & bindings
    GLuint id = 1;
    GLuint bindingIndex = 0;
    GLenum target = GL_SHADER_STORAGE_BUFFER; // TODO: Need this? Maybe use this
                                              // to reporpose

    // Pointer to data
    void *mappedPtr = nullptr;

    // Data size
    size_t size = 0;
    size_t elementSize = 0;
};

} // namespace Engine
