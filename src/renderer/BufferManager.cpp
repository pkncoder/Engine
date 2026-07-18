#include "BufferManager.h"

#include "../services/Logger.h"

#include <cstring>

namespace Engine {

void PersistentBuffer::setup(const GLenum bufferTarget,
                             const size_t bufferSize) {
    // Check if the buffer was already craeted
    if (id != 0)
        shutdown(); // Clean up if re-allocating

    // Set saved attributes
    target = bufferTarget;
    size = bufferSize;

    // Create and bind the buffer
    glGenBuffers(1, &id);
    glBindBuffer(target, id);

    // Buffer flags
    GLbitfield flags =
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    // Allocate the storage
    glBufferStorage(target, size, nullptr, flags);

    // Get the permanent CPU pointer
    mappedPtr = glMapBufferRange(target, 0, size, flags);

    // Unbind the buffer from the slot
    glBindBuffer(target, 0);
}

void PersistentBuffer::update(const void *data, const size_t updateSize) {

    // Check if the pointer is within range
    if (mappedPtr && data && updateSize <= size) {
        // Direct memory copy to the mapped VRAM pointer
        std::memcpy(mappedPtr, data, updateSize);
    } else {

        // Send error if out of bounds
        Logger::error(
            "SHADER",
            "PersistentBuffer Update Error: Out of bounds or unmapped");
    }
}

void PersistentBuffer::shutdown() {

    // Make sure the buffer was created
    if (id != 0) {

        // Unmap & delete the buffer
        glBindBuffer(target, id);
        glUnmapBuffer(target); // Unmap before deleting
        glDeleteBuffers(1, &id);

        // Reset attributes
        id = 0;
        mappedPtr = nullptr;
        size = 0;
        elementSize = 0;
    }
}

// Takes CPU data, pushes to VRAM, returns ECS-ready component
MeshComponent BufferManager::uploadMesh(const CPUMeshData &meshData) {

    // Check to see if this has already been uploaded
    if (gpuMeshCache.find(meshData.name) != gpuMeshCache.end()) {
        // Return the existing mesh component
        Logger::info("ASSET", "Returning cached mesh: " +
                                  meshData.name.substr(
                                      0, meshData.name.find_first_of("#")));
        return gpuMeshCache[meshData.name];
    }

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

    // Save the mesh data to the cache
    gpuMeshCache[meshData.name] = comp;

    // Return the mesh component previously made
    return comp;
}

void BufferManager::createUBO(const std::string &uboName,
                              const uint32_t bindingPoint,
                              const size_t totalBlockSize) {

    // Check to see if the UBO has already been created
    if (uboRegistry.find(uboName) != uboRegistry.end())
        return;

    // Create the ubo and set attributes
    GPUUniformBuffer ubo;
    ubo.bindingPoint = bindingPoint;
    ubo.size = totalBlockSize;
    ubo.cpuCache.resize(totalBlockSize, 0);

    // Generate the UBO
    glGenBuffers(1, &ubo.id);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);

    // Set the buffer size & binding point
    glBufferData(GL_UNIFORM_BUFFER, totalBlockSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo.id);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Save the ubo to the registry
    uboRegistry[uboName] = ubo;
}

void BufferManager::mapUBOLayout(const std::string &uboName,
                                 const uint32_t programID,
                                 const std::string &blockName,
                                 const std::vector<std::string> &uniformNames) {

    // Try to find the ubo
    auto it = uboRegistry.find(uboName);
    if (it == uboRegistry.end()) {
        Logger::error("BUFFER_MGR",
                      "Cannot map layout to unallocated UBO channel: " +
                          uboName);
        return;
    }

    // Get the found UBO
    GPUUniformBuffer &ubo = it->second;

    // Link shader programmatic location index to global physical binding port
    // Get the index & block name of the uniform block
    GLuint blockIndex = glGetUniformBlockIndex(programID, blockName.c_str());
    if (blockIndex == GL_INVALID_INDEX)
        return; // Pass might not use this block, ignore safely

    // Set the binding point at the index
    glUniformBlockBinding(programID, blockIndex, ubo.bindingPoint);

    // Find the uniform offsets
    for (const auto &name : uniformNames) {

        // Querry the indice
        const char *nameCStr = name.c_str();
        GLuint uniformIndex;
        glGetUniformIndices(programID, 1, &nameCStr, &uniformIndex);

        if (uniformIndex != GL_INVALID_INDEX) {

            // Get the uniform offset & save it to the ubo
            GLint offset = 0;
            glGetActiveUniformsiv(programID, 1, &uniformIndex,
                                  GL_UNIFORM_OFFSET, &offset);
            ubo.uniformOffsets[name] = static_cast<size_t>(offset);
        }
    }
}

void BufferManager::setUBOValue(const std::string &uboName,
                                const std::string &uniformName,
                                const void *data, const size_t dataSize) {

    // Look to see if the ubo exists
    auto it = uboRegistry.find(uboName);
    if (it == uboRegistry.end())
        return;

    // Get the offset of the uniform
    GPUUniformBuffer &ubo = it->second;
    auto offsetIt = ubo.uniformOffsets.find(uniformName);
    if (offsetIt == ubo.uniformOffsets.end())
        return; // Shader doesn't actively use this variable

    // Update the ubo cpuCache based on offset
    size_t targetOffset = offsetIt->second;
    if (targetOffset + dataSize <= ubo.size) { // Check for no bound error
        std::memcpy(ubo.cpuCache.data() + targetOffset, data, dataSize);
    }
}

void BufferManager::pushUBO(const std::string &uboName) {

    // Try to find the ubo in the registry
    auto it = uboRegistry.find(uboName);
    if (it == uboRegistry.end())
        return;

    // Subsitue the new data
    // TODO: offsets / windows
    const GPUUniformBuffer &ubo = it->second;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo.size, ubo.cpuCache.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BufferManager::shutdownUBORegistry() {

    // Loop each UBO and delete it's buffer
    for (auto &[name, ubo] : uboRegistry) {
        if (ubo.id != 0) {
            glDeleteBuffers(1, &ubo.id);
        }
    }

    // Clear remaining data
    uboRegistry.clear();
}

} // namespace Engine
