#pragma once

#include "BufferManager.h"

#include "../resources/AssetTypes.h"
#include "../scene/components/MeshComponent.h"

namespace Engine {

// Takes CPU data, pushes to VRAM, returns ECS-ready component
MeshComponent BufferManager::uploadMesh(const MeshData &meshData) {
    MeshComponent comp;
    comp.IndexCount = meshData.indices.size();

    // 1. Generate OpenGL Buffers
    glGenVertexArrays(1, &comp.VAO);
    glGenBuffers(1, &comp.VBO);
    glGenBuffers(1, &comp.EBO);

    // 2. Bind and Upload Data
    glBindVertexArray(comp.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, comp.VBO);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(Vertex),
                 meshData.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, comp.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.indices.size() * sizeof(uint32_t),
                 meshData.indices.data(), GL_STATIC_DRAW);

    // 3. Set Vertex Attributes (Position, Normal, TexCoord)

    // Enabling attributes & setting pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));

    // // Enabling attributes & setting pointers
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
    //                       (void *)offsetof(Vertex, color));

    // 4. Unbind VAO to prevent accidental modification
    glBindVertexArray(0);

    return comp;
}

} // namespace Engine
