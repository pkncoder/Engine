#include "BufferManager.h"

#include "../../services/Logger.h"
#include "../../services/UUID.h"
#include "GPUBuffer.h"

#include <cstring>
#include <unordered_map>
#include <vector>

namespace Engine {

BufferManager::~BufferManager() {}

BufferHandle
BufferManager::createBuffer(const std::string &name, const BufferType type,
                            const BufferUsage usage, const size_t size,
                            const void *initialData, const bool multiBuffered) {

    // Setup the new buffer & attributes
    GPUBuffer buffer;
    buffer.name = name;
    buffer.handle = UUID();

    buffer.usage = usage;

    buffer.size = size;
    buffer.type = type;

    buffer.multiBuffered = multiBuffered;

    buffer.cpuCache = std::vector<uint8_t>();

    if (type == BufferType::UniformBuffer) {
        buffer.uniformOffsets = std::unordered_map<std::string, size_t>();
    }

    // Generate as many gl buffers as needed & asign the ids
    uint32_t buffersToGenerate = multiBuffered ? FRAMES_IN_FLIGHT : 1;
    glGenBuffers(buffersToGenerate, buffer.ids);

    // Get the opengl type & usage of the buffers
    GLenum glType = static_cast<GLenum>(type);
    GLenum glUsage = static_cast<GLenum>(usage);

    // Loop over each buffer to generate
    for (uint32_t i = 0; i < buffersToGenerate; i++) {
        // Bind the buffer & the initial data
        glBindBuffer(glType, buffer.ids[i]);
        glBufferData(glType, size, initialData, glUsage);
    }

    // Unbind the buffer
    glBindBuffer(glType, 0);

    // Save the buffer into the registry
    bufferRegistry[buffer.handle] = std::move(buffer);

    // If the buffer is dynamic, resize the cpu cache
    if (usage == BufferUsage::Dynamic) {
        buffer.cpuCache.resize(size, 0);
        if (initialData) { // memcpy any initial data if it exists
            std::memcpy(buffer.cpuCache.data(), initialData, size);
        }
    }

    return buffer.handle;
}

void BufferManager::destroyBuffer(const BufferHandle handle) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER",
                      "Buffer not found. BufferManager::destroyBuffer");
        return;
    }

    // Get the refrence
    GPUBuffer &buffer = ittr->second;

    // Figure out how many buffers to generate & delete them
    size_t numBuffers = buffer.multiBuffered ? FRAMES_IN_FLIGHT : 1;
    glDeleteBuffers(numBuffers, buffer.ids);

    // Erase the handle & buffer, freeing the memory
    bufferRegistry.erase(handle);
}

GPUBuffer *BufferManager::getBuffer(const BufferHandle handle) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER", "Failed to find buffer : getBuffer");
    }

    // Return a refrence
    return &ittr->second;
}

GPUBuffer *BufferManager::getBufferByName(const std::string &name) {

    // Loop over each buffer in the registry
    for (auto &[handle, buffer] : bufferRegistry) {

        // Try to match the name, if found return the buffer
        if (buffer.name == name) {
            return &buffer;
        }
    }

    // No buffer matched
    Logger::error("BUFFER", "Buffer not found by name");

    return nullptr;
}

void BufferManager::updateBufferCache(const BufferHandle handle,
                                      const size_t offset, const size_t size,
                                      const void *data) {

    // Find target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER",
                      "Buffer not found. BufferManager::updateBufferCache");
        return;
    }

    // Obtain a refrence
    GPUBuffer &buffer = ittr->second;

    // Check for out of bounds error
    if (offset + size <= buffer.size) {
        Logger::error("BUFFER", "Buffer Out-Of-Bounds error prevented "
                                "BufferManager::updateBufferCache");
        return;
    }

    // memcpy the data to the cpu cache
    std::memcpy(buffer.cpuCache.data() + offset, data, size);
}

void BufferManager::mapUBOLayout(const BufferHandle handle,
                                 const uint32_t programID,
                                 const std::string &blockName,
                                 const std::vector<std::string> &uniformNames) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Cannot map layout to unallocated UBO handle: " +
                          std::to_string(handle));
        return;
    }

    // Get the refrence
    GPUBuffer &buffer = ittr->second;

    // Make sure this buffer is actually a UBO
    if (buffer.type != BufferType::UniformBuffer) {
        Logger::error("BUFFER_MGR",
                      "Attempted to map UBO layout on a non-UBO buffer: " +
                          buffer.name);
        return;
    }

    // Get the block index from the program
    GLuint blockIndex = glGetUniformBlockIndex(programID, blockName.c_str());
    if (blockIndex == GL_INVALID_INDEX) {
        // Ignore the unused block
        return;
    }

    // Loop each name in uniform names
    for (const auto &name : uniformNames) {

        // Get the cStr of the name & create a variable for the uniform index
        const char *nameCStr = name.c_str();
        GLuint uniformIndex;

        // Query OpenGL for the specific index of this variable
        glGetUniformIndices(programID, 1, &nameCStr, &uniformIndex);

        // Make sure the index is valid
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

void BufferManager::setUBOValue(const BufferHandle handle,
                                const std::string &uniformName,
                                const size_t size, const void *data) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end())
        return;

    // Get the refrence
    GPUBuffer &buffer = ittr->second;

    // Find the target uniform offset
    auto offsetIttr = buffer.uniformOffsets.find(uniformName);
    if (offsetIttr == buffer.uniformOffsets.end()) {
        // Ignore a not found uniform
        return;
    }

    // Get the offset
    size_t targetOffset = offsetIttr->second;

    // Check the bounds of the UBO
    if (targetOffset + size <= buffer.size) {
        // Copy the new data into the CPU cache
        std::memcpy(buffer.cpuCache.data() + targetOffset, data, size);
    } else {
        Logger::error("BUFFER_MGR",
                      "Buffer overflow prevented in UBO: " + buffer.name +
                          " for uniform: " + uniformName);
    }
}

void BufferManager::bindBuffer(const BufferHandle handle,
                               const uint32_t globalFrameIndex) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Attempted to bind invalid buffer handle: " +
                          std::to_string(handle));
        return;
    }

    // Get a refrence
    const GPUBuffer &buffer = ittr->second;

    // Get the current id based on the global frame num
    GLuint activeID = buffer.multiBuffered
                          ? buffer.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                          : buffer.ids[0];

    // Bind the buffer
    glBindBuffer(static_cast<GLenum>(buffer.type), activeID);
}

void BufferManager::bindBufferBase(const BufferHandle handle,
                                   const uint32_t bindingPoint,
                                   const uint32_t globalFrameIndex) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Attempted to bind invalid buffer handle: " +
                          std::to_string(handle));
        return;
    }

    // Obtain a refrence
    const GPUBuffer &buffer = ittr->second;

    // Get the current id based on the global frame num
    GLuint activeID = buffer.multiBuffered
                          ? buffer.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                          : buffer.ids[0];

    // Bind the base of the buffer
    glBindBufferBase(static_cast<GLenum>(buffer.type), bindingPoint, activeID);
}

void BufferManager::streamData(const BufferHandle handle, const size_t size,
                               const void *data,
                               const uint32_t globalFrameIndex) {

    // Find target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end())
        return;

    // Obtain a refrence
    const GPUBuffer &buffer = ittr->second;

    // Make sure this buffer is actually a stream buffer
    if (buffer.usage != BufferUsage::Stream) {
        Logger::error("BUFFER_MGR",
                      "Attempted to streamBuffer on a non-stream buffer: " +
                          buffer.name);
        return;
    }

    // Check for out of bounds errors
    if (size > buffer.size) {
        Logger::error("BUFFER_MGR",
                      "Stream size exceeds buffer capacity for: " +
                          buffer.name);
        return;
    }

    // Resolve the active frame ID based on the global frame number
    GLuint activeID = buffer.multiBuffered
                          ? buffer.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                          : buffer.ids[0];
    GLenum glType = static_cast<GLenum>(buffer.type);

    // Bind the current buffer
    glBindBuffer(glType, activeID);

    // Pass null to the data to reset the current memory block
    glBufferData(glType, buffer.size, nullptr, GL_STREAM_DRAW);

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
                               const uint32_t globalFrameIndex) {

    // Find the target buffer
    auto ittr = bufferRegistry.find(handle);
    if (ittr == bufferRegistry.end())
        return;

    // Get a refrence to the bfufer
    const GPUBuffer &buffer = ittr->second;

    // Make sure this buffer is a dynamic buffer
    if (buffer.usage != BufferUsage::Dynamic) {
        Logger::error("BUFFER_MGR",
                      "Attempted to pushBuffer on a non-dynamic buffer: " +
                          buffer.name);
        return;
    }

    // Get the active buffer id based on the global frame num
    GLuint activeID = buffer.multiBuffered
                          ? buffer.ids[globalFrameIndex % FRAMES_IN_FLIGHT]
                          : buffer.ids[0];
    GLenum glType = static_cast<GLenum>(buffer.type);

    Logger::debug(std::to_string(buffer.ids[0]) + " ; " +
                  std::to_string(buffer.ids[0]));

    // Bind, push the CPU cache, and unbind
    glBindBuffer(glType, activeID);
    glBufferSubData(glType, 0, buffer.size, buffer.cpuCache.data());
    glBindBuffer(glType, 0);
}

} // namespace Engine
