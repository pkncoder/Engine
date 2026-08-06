#include "BufferManager.h"

#include "../../services/Logger.h"
#include "UBO.h"

#include <cstring>

namespace Engine {

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

    auto createUBOBuffer = [&](GLuint &uboId) {
        glGenBuffers(1, &uboId);
        glBindBuffer(GL_UNIFORM_BUFFER, uboId);

        glBufferData(GL_UNIFORM_BUFFER, totalBlockSize, nullptr,
                     GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboId);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    };

    createUBOBuffer(ubo.frontBuffer);
    createUBOBuffer(ubo.backBuffer);

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
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.backBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo.size, ubo.cpuCache.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BufferManager::shutdownUBORegistry() {

    // Loop each UBO and delete it's buffer
    for (auto &[name, ubo] : uboRegistry) {
        if (ubo.frontBuffer != 0) {
            glDeleteBuffers(1, &ubo.frontBuffer);
        }

        if (ubo.backBuffer != 0) {
            glDeleteBuffers(1, &ubo.backBuffer);
        }
    }

    // Clear remaining data
    uboRegistry.clear();
}

} // namespace Engine
