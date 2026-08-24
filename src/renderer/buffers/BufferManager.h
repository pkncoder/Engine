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
    static BufferHandle createBuffer(const std::string &name,
                                     const BufferType type,
                                     const BufferUsage usage, const size_t size,
                                     const void *initialData = nullptr,
                                     const bool multiBuffered = false);
    static void destroyBuffer(const BufferHandle handle);

    static GPUBuffer *getBuffer(const BufferHandle handle);
    static GPUBuffer *getBufferByName(const std::string &name);

    static void updateBufferCache(const BufferHandle handle, const size_t size,
                                  const size_t offset, const void *data);

    static void mapUBO(const BufferHandle handle, const uint32_t programID,
                       const std::string &blockName,
                       const std::vector<std::string> &uniformNames);
    static void setUBOValue(const BufferHandle handle,
                            const std::string &uniformName, const size_t size,
                            const void *data);

    static void streamData(const BufferHandle handle, const size_t size,
                           const void *data, const uint32_t globalFrameIndex);

    static void pushBuffer(const BufferHandle handle,
                           const uint32_t globalFrameIndex);

  private:
    static inline GLuint nextBindingIndex = 0;
    static inline std::unordered_map<BufferHandle, GPUBuffer> bufferRegistry;
};

} // namespace Engine
