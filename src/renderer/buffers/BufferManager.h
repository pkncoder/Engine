#pragma once

#include "GPUBuffer.h"

#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef>

namespace Engine {

class BufferManager {
  public:
    BufferManager() = default;
    ~BufferManager();

    // Creation and deletion of buffers
    BufferHandle createBuffer(const std::string &name, BufferType type,
                              BufferUsage usage, size_t size,
                              const void *initialData = nullptr,
                              bool multiBuffered = false);
    void destroyBuffer(BufferHandle handle);

    void updateBufferCache(BufferHandle handle, size_t size, size_t offset,
                           const void *data);

    void pushBuffer(BufferHandle handle, uint32_t globalFrameIndex);

    void streamData(BufferHandle handle, size_t size, const void *data,
                    uint32_t globalFrameIndex);

    void bindBuffer(BufferHandle handle, uint32_t globalFrameIndex);
    void bindBufferBase(BufferHandle handle, uint32_t bindingPoint,
                        uint32_t globalFrameIndex);

    void mapUBOLayout(BufferHandle handle, uint32_t programID,
                      const std::string &blockName,
                      const std::vector<std::string> &uniformNames);
    void setUBOLayout(BufferHandle handle, const std::string &uniformName,
                      size_t size, const void *data);

    GPUBuffer *getBuffer(BufferHandle handle);
    GPUBuffer *getBufferByName(const std::string &name);

  private:
    static inline std::unordered_map<BufferHandle, GPUBuffer> bufferRegistry;
};

} // namespace Engine
