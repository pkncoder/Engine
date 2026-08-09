#include "BufferManager.h"

#include "../../services/Logger.h"
#include "../../services/UUID.h"
#include "GPUBuffer.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Engine {

BufferManager::~BufferManager() {}

BufferHandle
BufferManager::createBuffer(const std::string &name, const BufferType type,
                            const BufferUsage usage, const size_t size,
                            const void *initialData, const bool multiBuffered) {

    GPUBuffer buffer;
    buffer.name = name;
    buffer.handle = UUID();

    buffer.size = size;
    buffer.type = type;

    buffer.multiBuffered = multiBuffered;

    buffer.cpuCache = std::vector<uint8_t>();

    if (type == BufferType::UniformBuffer) {
        buffer.uniformOffsets = std::unordered_map<std::string, size_t>();
    }

    uint32_t buffersToGenerate = multiBuffered ? FRAMES_IN_FLIGHT : 1;
    glGenBuffers(buffersToGenerate, buffer.ids);

    GLenum glType = static_cast<GLenum>(type);
    GLenum glUsage = static_cast<GLenum>(usage);

    for (uint32_t i = 0; i < buffersToGenerate; i++) {
        glBindBuffer(glType, buffer.ids[i]);
        glBufferData(glType, size, initialData, glUsage);

        glBindBuffer(glType, 0);

        bufferRegistry[buffer.handle] = std::move(buffer);
    }

    if (usage == BufferUsage::Dynamic) {
        buffer.cpuCache.resize(size, 0);
        if (initialData) {
            std::memcpy(buffer.cpuCache.data(), initialData, size);
        }
    }

    return buffer.handle;
}

void BufferManager::destroyBuffer(const BufferHandle handle) {

    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER",
                      "Buffer not found. BufferManager::destroyBuffer");
        return;
    }

    GPUBuffer &buffer = ittr->second;

    size_t numBuffers = buffer.multiBuffered ? FRAMES_IN_FLIGHT : 1;
    glDeleteBuffers(numBuffers, buffer.ids);

    bufferRegistry.erase(handle);
}

GPUBuffer *BufferManager::getBuffer(const BufferHandle handle) {
    auto ittr = bufferRegistry.find(handle);

    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER", "Failed to find buffer : getBuffer");
    }

    return &ittr->second;
}

GPUBuffer *BufferManager::getBufferByName(const std::string &name) {

    for (auto &[handle, buffer] : bufferRegistry) {
        if (buffer.name == name) {
            return &buffer;
        }
    }

    Logger::error("BUFFER", "Buffer not found by name");

    return nullptr;
}

void BufferManager::updateBufferCache(const BufferHandle handle,
                                      const size_t offset, const size_t size,
                                      const void *data) {
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER",
                      "Buffer not found. BufferManager::updateBufferCache");
        return;
    }

    GPUBuffer &buffer = ittr->second;

    if (offset + size < buffer.size) {
        Logger::error("BUFFER", "Buffer Out-Of-Bounds error prevented "
                                "BufferManager::updateBufferCache");
        return;
    }

    std::memcpy(buffer.cpuCache.data() + offset, data, size);
}

void BufferManager::mapUBOLayout(const BufferHandle handle,
                                 const uint32_t programID,
                                 const std::string &blockName,
                                 const std::vector<std::string> &uniformNames) {

    auto it = bufferRegistry.find(handle);
    if (it == bufferRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Cannot map layout to unallocated UBO handle: " +
                          std::to_string(handle));
        return;
    }

    GPUBuffer &buffer = it->second;

    // Safety check: Ensure this is actually a UBO
    if (buffer.type != BufferType::UniformBuffer) {
        Logger::error("BUFFER_MGR",
                      "Attempted to map UBO layout on a non-UBO buffer: " +
                          buffer.name);
        return;
    }

    // 2. Get the index of the uniform block within the shader program
    GLuint blockIndex = glGetUniformBlockIndex(programID, blockName.c_str());
    if (blockIndex == GL_INVALID_INDEX) {
        // The shader compiler often optimizes out unused blocks. Don't crash,
        // just ignore.
        return;
    }
    for (const auto &name : uniformNames) {
        const char *nameCStr = name.c_str();
        GLuint uniformIndex;

        // Query OpenGL for the specific index of this variable
        glGetUniformIndices(programID, 1, &nameCStr, &uniformIndex);

        if (uniformIndex != GL_INVALID_INDEX) {
            GLint offset = 0;
            // Query OpenGL for the byte offset of that index
            glGetActiveUniformsiv(programID, 1, &uniformIndex,
                                  GL_UNIFORM_OFFSET, &offset);

            // Cache the offset in the GPUBuffer struct
            buffer.uniformOffsets[name] = static_cast<size_t>(offset);
        }
    }
}

void BufferManager::setUBOLayout(const BufferHandle handle,
                                 const std::string &uniformName,
                                 const size_t size, const void *data) {
    auto it = bufferRegistry.find(handle);
    if (it == bufferRegistry.end())
        return;

    GPUBuffer &buffer = it->second;

    // 2. Look up the cached byte offset for this specific variable
    auto offsetIt = buffer.uniformOffsets.find(uniformName);
    if (offsetIt == buffer.uniformOffsets.end()) {
        // Variable wasn't found (likely optimized out by the GLSL compiler).
        // Safely ignore.
        return;
    }

    size_t targetOffset = offsetIt->second;

    // 3. Safety bounds check before modifying memory
    if (targetOffset + size <= buffer.size) {
        // Copy the new data into the CPU cache shadow copy
        std::memcpy(buffer.cpuCache.data() + targetOffset, data, size);
    } else {
        Logger::error("BUFFER_MGR",
                      "Buffer overflow prevented in UBO: " + buffer.name +
                          " for uniform: " + uniformName);
    }
}

void BufferManager::bindBuffer(const BufferHandle handle,
                               const uint32_t globalFrameIndex) const {
    auto it = bufferRegistry.find(handle);
    if (it == bufferRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Attempted to bind invalid buffer handle: " +
                          std::to_string(handle));
        return;
    }

    const GPUBuffer &buf = it->second;

    // Dynamically resolve the correct OpenGL ID based on the current frame
    GLuint activeID = buf.multiBuffered
                          ? buf.ids[globalFrameIndex & FRAMES_IN_FLIGHT]
                          : buf.ids[0];

    glBindBuffer(static_cast<GLenum>(buf.type), activeID);
}

void BufferManager::bindBufferBase(const BufferHandle handle,
                                   const uint32_t bindingPoint,
                                   const uint32_t globalFrameIndex) const {
    auto it = bufferRegistry.find(handle);
    if (it != bufferRegistry.end()) {
        const GPUBuffer &buf = it->second;

        GLuint activeID = buf.multiBuffered
                              ? buf.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                              : buf.ids[0];

        glBindBufferBase(static_cast<GLenum>(buf.type), bindingPoint, activeID);
    }
}

void BufferManager::streamData(const BufferHandle handle, const size_t size,
                               const void *data,
                               const uint32_t globalFrameIndex) const {
    auto it = bufferRegistry.find(handle);
    if (it == bufferRegistry.end())
        return;

    const GPUBuffer &buf = it->second;

    // Safety check: Only Stream buffers use this high-frequency update path
    if (buf.usage != BufferUsage::Stream) {
        Logger::error("BUFFER_MGR",
                      "Attempted to streamBuffer on a non-stream buffer: " +
                          buf.name);
        return;
    }

    // Ensure we don't write out of bounds
    if (size > buf.size) {
        Logger::error("BUFFER_MGR",
                      "Stream size exceeds buffer capacity for: " + buf.name);
        return;
    }

    // Resolve the active frame ID
    GLuint activeID = buf.multiBuffered
                          ? buf.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                          : buf.ids[0];
    GLenum glType = static_cast<GLenum>(buf.type);

    glBindBuffer(glType, activeID);

    // OpenGL 4.1 Buffer Orphaning:
    // Pass nullptr to force the driver to give us a brand new block of VRAM
    // so we don't stall waiting for the GPU to finish reading the old block.
    glBufferData(glType, buf.size, nullptr, GL_STREAM_DRAW);

    // Map the new memory block, copy the data, and unmap
    void *mappedPtr = glMapBufferRange(
        glType, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (mappedPtr) {
        std::memcpy(mappedPtr, data, size);
        glUnmapBuffer(glType);
    }

    glBindBuffer(glType, 0);
}

void BufferManager::pushBuffer(const BufferHandle handle,
                               const uint32_t globalFrameIndex) const {
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end())
        return;

    const GPUBuffer &buf = ittr->second;

    // Safety check: Only Dynamic buffers use the CPU cache pushing mechanism
    if (buf.usage != BufferUsage::Dynamic) {
        Logger::error("BUFFER_MGR",
                      "Attempted to pushBuffer on a non-dynamic buffer: " +
                          buf.name);
        return;
    }

    // Resolve the active frame ID
    GLuint activeID = buf.multiBuffered
                          ? buf.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                          : buf.ids[0];
    GLenum glType = static_cast<GLenum>(buf.type);

    // Bind, push the shadowed CPU cache, and unbind
    glBindBuffer(glType, activeID);
    glBufferSubData(glType, 0, buf.size, buf.cpuCache.data());
    glBindBuffer(glType, 0);
}

} // namespace Engine
