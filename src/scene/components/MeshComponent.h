#pragma once

#include "../../resources/CPUStructs.h"

#include <glad/glad.h>

#include <string>

namespace Engine {

// Mesh component struct
struct MeshComponent {
  public:
    // Name declaration
    std::string assetID = "";

    // Mesh info
    CPUMeshData meshData;

    // OpenGL arrays
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    // Index count
    unsigned int indexCount = 0;
};

} // namespace Engine
