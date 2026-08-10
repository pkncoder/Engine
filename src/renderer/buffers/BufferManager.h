#pragma once

#include "GPUBuffer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {

class BufferManager {
  public:
    BufferManager() = default;
    ~BufferManager();

    // Creation and deletion of buffers
    BufferHandle createBuffer(const std::string &name, const BufferType type,
                              const BufferUsage usage, const size_t size,
                              const void *initialData = nullptr,
                              const bool multiBuffered = false);
    void destroyBuffer(const BufferHandle handle);

    GPUBuffer *getBuffer(const BufferHandle handle);
    GPUBuffer *getBufferByName(const std::string &name);

    void updateBufferCache(const BufferHandle handle, const size_t size,
                           const size_t offset, const void *data);

    void mapUBOLayout(const BufferHandle handle, const uint32_t programID,
                      const std::string &blockName,
                      const std::vector<std::string> &uniformNames);
    void setUBOLayout(const BufferHandle handle, const std::string &uniformName,
                      const size_t size, const void *data);

    void bindBuffer(const BufferHandle handle,
                    const uint32_t globalFrameIndex) const;
    void bindBufferBase(const BufferHandle handle, const uint32_t bindingPoint,
                        const uint32_t globalFrameIndex) const;

    void streamData(const BufferHandle handle, const size_t size,
                    const void *data, const uint32_t globalFrameIndex) const;

    void pushBuffer(const BufferHandle handle,
                    const uint32_t globalFrameIndex) const;

  private:
    std::unordered_map<BufferHandle, GPUBuffer> bufferRegistry;
};

} // namespace Engine
