#include "SSBO.h"

#include "../../services/Logger.h"

namespace Engine {

void SSBO::setup(const GLenum bufferTarget, const size_t bufferSize) {
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

void SSBO::update(const void *data, const size_t updateSize) {

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

void SSBO::shutdown() {

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

}; // namespace Engine
