#pragma once

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

using BufferHandle = uint32_t;
constexpr BufferHandle INVALID_BUFFER_HANDLE = 0;

constexpr size_t FRAMES_IN_FLIGHT = 2;

enum class BufferUsage {
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
    Stream = GL_STREAM_DRAW
};

enum class BufferType {
    UniformBuffer = GL_UNIFORM_BUFFER,
    StorageBuffer = GL_SHADER_STORAGE_BUFFER,
    VertexBuffer = GL_ARRAY_BUFFER,
    IndexBuffer = GL_ELEMENT_ARRAY_BUFFER,
    TextureBuffer = GL_TEXTURE_BUFFER
};

struct GPUBuffer {
  public:
    // Handle and string name
    BufferHandle handle = INVALID_BUFFER_HANDLE;
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
