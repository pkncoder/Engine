#include "BufferManager.h"

#include "../services/Logger.h"

#include <cstring> // for memcpy

namespace Engine {

// Takes CPU data, pushes to VRAM, returns ECS-ready component
MeshComponent BufferManager::uploadMesh(const CPUMeshData &meshData) {

    if (gpuMeshCache.find(meshData.name) != gpuMeshCache.end()) {
        // Return the existing mesh component
        return gpuMeshCache[meshData.name];
    }
    // TODO: reimplement caching

    // Create the new mesh component and set the index count
    MeshComponent comp;
    comp.indexCount = meshData.indices.size();
    comp.assetID = meshData.name;
    comp.meshData = meshData;

    // Generating arrays and buffers
    glGenVertexArrays(1, &comp.vao);
    glGenBuffers(1, &comp.vbo);
    glGenBuffers(1, &comp.ebo);

    // Bind the vertex array data
    glBindVertexArray(comp.vao);

    // Bind and set the data for the vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, comp.vbo);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(CPUVertex),
                 meshData.vertices.data(), GL_STATIC_DRAW);

    // Bind the element buffer and data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, comp.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.indices.size() * sizeof(uint32_t),
                 meshData.indices.data(), GL_STATIC_DRAW);

    // Vertex attribute setting (position)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CPUVertex),
                          (void *)offsetof(CPUVertex, position));

    // Vertex attribute setting (normal)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CPUVertex),
                          (void *)offsetof(CPUVertex, normal));

    // Vertex attribute setting (texture)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CPUVertex),
                          (void *)offsetof(CPUVertex, texCoords));

    // Unbind the vertex array for storage reasongs
    glBindVertexArray(0);

    gpuMeshCache[meshData.name] = comp;

    // Return the mesh component previously made
    return comp;
}

void PersistentBuffer::setup(GLenum bufferTarget, size_t bufferSize) {
    if (id != 0)
        shutdown(); // Clean up if re-allocating

    target = bufferTarget;
    size = bufferSize;

    glGenBuffers(1, &id);
    glBindBuffer(target, id);

    // GL_MAP_COHERENT_BIT means the GPU automatically sees CPU changes without
    // us needing to call glFlushMappedBufferRange
    GLbitfield flags =
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    // Allocate immutable storage
    glBufferStorage(target, size, nullptr, flags);

    // Get the permanent CPU pointer
    mappedPtr = glMapBufferRange(target, 0, size, flags);

    glBindBuffer(target, 0);
}

void PersistentBuffer::update(const void *data, size_t updateSize) {
    if (mappedPtr && data && updateSize <= size) {
        // Direct memory copy to the mapped VRAM pointer. Lightning fast.
        std::memcpy(mappedPtr, data, updateSize);
    } else {
        Logger::error(
            "SHADER",
            "PersistentBuffer Update Error: Out of bounds or unmapped");
    }
}

void PersistentBuffer::shutdown() {
    if (id != 0) {
        glBindBuffer(target, id);
        glUnmapBuffer(target); // Unmap before deleting
        glDeleteBuffers(1, &id);
        id = 0;
        mappedPtr = nullptr;
        size = 0;
        elementSize = 0;
    }
}

void BufferManager::createUBO(const std::string &uboName, uint32_t bindingPoint,
                              size_t totalBlockSize) {
    if (uboRegistry.find(uboName) != uboRegistry.end())
        return;

    GPUUniformBuffer ubo;
    ubo.bindingPoint = bindingPoint;
    ubo.size = totalBlockSize;
    ubo.cpuCache.resize(totalBlockSize, 0);

    glGenBuffers(1, &ubo.id);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);
    glBufferData(GL_UNIFORM_BUFFER, totalBlockSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo.id);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    uboRegistry[uboName] = ubo;
}

void BufferManager::mapUBOLayout(const std::string &uboName, uint32_t programID,
                                 const std::string &blockName,
                                 const std::vector<std::string> &uniformNames) {
    auto it = uboRegistry.find(uboName);
    if (it == uboRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Cannot map layout to unallocated UBO channel: " +
                          uboName);
        return;
    }

    GPUUniformBuffer &ubo = it->second;

    // Link shader programmatic location index to global physical binding port
    GLuint blockIndex = glGetUniformBlockIndex(programID, blockName.c_str());
    if (blockIndex == GL_INVALID_INDEX)
        return; // Pass might not use this block, ignore safely

    glUniformBlockBinding(programID, blockIndex, ubo.bindingPoint);

    // Query offsets using 4.1 core reflection functions
    for (const auto &name : uniformNames) {
        const char *nameCStr = name.c_str();
        GLuint uniformIndex;
        glGetUniformIndices(programID, 1, &nameCStr, &uniformIndex);

        if (uniformIndex != GL_INVALID_INDEX) {
            GLint offset = 0;
            glGetActiveUniformsiv(programID, 1, &uniformIndex,
                                  GL_UNIFORM_OFFSET, &offset);
            ubo.uniformOffsets[name] = static_cast<size_t>(offset);
        }
    }
}

void BufferManager::setUBOValue(const std::string &uboName,
                                const std::string &uniformName,
                                const void *data, size_t dataSize) {
    auto it = uboRegistry.find(uboName);
    if (it == uboRegistry.end())
        return;

    // TODO: check with the cpuCache to see if there is a match

    GPUUniformBuffer &ubo = it->second;
    auto offsetIt = ubo.uniformOffsets.find(uniformName);
    if (offsetIt == ubo.uniformOffsets.end())
        return; // Shader doesn't actively use this variable

    size_t targetOffset = offsetIt->second;
    if (targetOffset + dataSize <= ubo.size) {
        std::memcpy(ubo.cpuCache.data() + targetOffset, data, dataSize);
    }
}

void BufferManager::pushUBO(const std::string &uboName) {
    auto it = uboRegistry.find(uboName);
    if (it == uboRegistry.end())
        return;

    const GPUUniformBuffer &ubo = it->second;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo.size, ubo.cpuCache.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BufferManager::shutdownRegistry() {
    for (auto &[name, ubo] : uboRegistry) {
        if (ubo.id != 0) {
            glDeleteBuffers(1, &ubo.id);
        }
    }
    uboRegistry.clear();
}

} // namespace Engine
