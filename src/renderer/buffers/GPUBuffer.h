#pragma once

#include <glad/glad.h>

#include <cstddef>
#include <cstdint>
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
    BufferHandle handle = INVALID_BUFFER_HANDLE; // TODO: Kepp or no
    std::string name;

    BufferUsage usage;
    BufferType type;

    bool multiBuffered = false;

    GLuint ids[FRAMES_IN_FLIGHT] = {0};
    size_t size = 0;

    std::vector<uint8_t> cpuCache;
    std::unordered_map<std::string, size_t> uniformOffsets;
};

} // namespace Engine
