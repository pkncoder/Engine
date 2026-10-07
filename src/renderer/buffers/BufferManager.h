#pragma once

#include "../gpuResources/GPUBuffer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {

class BufferManager {
  public:
    BufferManager() = default;
    ~BufferManager();

    // Creation and deletion of buffers
    static ResourceHandle createBuffer(const std::string &name,
                                     const BufferType type,
                                     const BufferUsage usage, const size_t size,
                                     const GLuint bindingIndex = -1,
                                     const void *initialData = nullptr,
                                     const bool multiBuffered = false);
    static void destroyBuffer(const ResourceHandle handle);

    // Buffer fetching
    static GPUBuffer *getBuffer(const ResourceHandle handle);
    static GPUBuffer *getBufferByName(const std::string &name);

    // Update the cpuBuffer cache directy
    static void updateBufferCache(const ResourceHandle handle, const size_t size,
                                  const size_t offset, const void *data);

    // Mapping and setting of buffer data
    static void mapUBO(const ResourceHandle handle, const uint32_t programID,
                       const std::string &blockName,
                       const std::vector<std::string> &uniformNames);
    static void setUBOValue(const ResourceHandle handle,
                            const std::string &uniformName, const size_t size,
                            const void *data);

    // Sending buffers to the gpu
    static void streamData(const ResourceHandle handle, const size_t size,
                           const void *data, const uint32_t globalFrameIndex);

    static void pushBuffer(const ResourceHandle handle,
                           const uint32_t globalFrameIndex);

  private:
    // Used for the dynamic binding system (MacOS compatability)
    static inline GLuint nextBindingIndex = 0;

    // Registry of every buffer
    static inline std::unordered_map<ResourceHandle, GPUBuffer> bufferRegistry;
};

} // namespace Engine
