#pragma once

#include "../enums/BufferType.h"
#include "../enums/BufferUsage.h"
#include "IGPUResource.h"

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

using ResourceHandle = uint32_t;
const ResourceHandle INVALID_RESOURCE_HANDLE = 0;

constexpr size_t FRAMES_IN_FLIGHT = 2;

struct GPUBufferDescriptor {
    const size_t size = 0;
    const void *initialData = nullptr;

    const GLuint bindingIndex = -1;

    const bool multiBuffered = false;
};

struct GPUBuffer : public IGPUResource {
  public:
    // Handle and string name
    std::string name;

    // Buffer type and use enums - static cast to GLenum
    BufferUsage usage;
    BufferType type;

    // Buffer ids are saved via an array for multibuffering - set by bool
    bool multiBuffered = false;
    GLuint ids[FRAMES_IN_FLIGHT] = {0};
    GLuint bindingIndex = -1;

    // Size of the buffer
    size_t size = 0;

    // CPU data caches
    std::vector<uint8_t> cpuCache;
    std::unordered_map<std::string, size_t>
        uniformOffsets; // Specifically for UBOs
};

} // namespace Engine
