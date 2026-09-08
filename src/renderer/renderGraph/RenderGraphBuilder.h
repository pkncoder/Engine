#pragma once

#include "../buffers/GPUBuffer.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace Engine {

using ResourceHandle = uint32_t;
constexpr ResourceHandle INVALID_RESOURCE_HANDLE = ~0;

enum ResourceType {
    Texture2D,
    TextureCube,
    UniformBuffer,
    TextureBufferObject,
    ShaderStorageBuffer
};

struct ResourceDescriptor {
    ResourceType type = ResourceType::Texture2D;

    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t glInternalFormat = 0;

    size_t sizeInBytes = 0;
};

class RenderGraphBuilder {
  public:
    virtual ~RenderGraphBuilder() = default;

    virtual void read(ResourceHandle handle) = 0;
    virtual void write(ResourceHandle handle) = 0;

    virtual ResourceHandle createTexture(const std::string &name,
                                         const ResourceDescriptor &desc) = 0;
    virtual ResourceHandle importBuffer(const std::string &name,
                                        BufferHandle existingBuffer) = 0;
};

} // namespace Engine
