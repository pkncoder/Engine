#include "BufferManager.h"

#include "glad/glad.h"

#include <cstddef>

namespace Engine {

// Creating a simple mesh - probally temp
uint32_t BufferManager::createSimpleMesh(const std::vector<Vertex> &vertices) {
    uint32_t vao, vbo; // Set spots for buffers

    // Generate the buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Bind the vao
    glBindVertexArray(vao);

    // Binding and setting buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    // Enabling attributes & setting pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));

    // Enabling attributes & setting pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, color));

    // Empty the binds for mem safety
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Returning the vao
    return vao;
}

} // namespace Engine
