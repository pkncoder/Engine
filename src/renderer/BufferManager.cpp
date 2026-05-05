#include "BufferManager.h"

#include <cstring> // for memcpy
#include <iostream>

namespace Engine {

// Takes CPU data, pushes to VRAM, returns ECS-ready component
MeshComponent BufferManager::uploadMesh(const MeshData &meshData) {

    // Create the new mesh component and set the index count
    MeshComponent comp;
    comp.indexCount = meshData.indices.size();

    // Generating arrays and buffers
    glGenVertexArrays(1, &comp.vao);
    glGenBuffers(1, &comp.vbo);
    glGenBuffers(1, &comp.ebo);

    // Bind the vertex array data
    glBindVertexArray(comp.vao);

    // Bind and set the data for the vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, comp.vbo);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(Vertex),
                 meshData.vertices.data(), GL_STATIC_DRAW);

    // Bind the element buffer and data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, comp.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.indices.size() * sizeof(uint32_t),
                 meshData.indices.data(), GL_STATIC_DRAW);

    // Vertex attribute setting (position)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));

    // Vertex attribute setting (normal)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));

    // Unbind the vertex array for storage reasongs
    glBindVertexArray(0);

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
        std::cerr
            << "PersistentBuffer Update Error: Out of bounds or unmapped.\n";
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
    }
}

} // namespace Engine
