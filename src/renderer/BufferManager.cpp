#include "BufferManager.h"

#include "../resources/AssetTypes.h"
#include "../scene/components/MeshComponent.h"

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

} // namespace Engine
