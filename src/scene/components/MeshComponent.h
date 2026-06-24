#pragma once

#include "../../resources/CPUStructs.h"

#include <glad/glad.h>

#include <string>

namespace Engine {

// Mesh component struct
struct MeshComponent {

    std::string assetID = "";

    CPUMeshData meshData;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    unsigned int indexCount = 0;
};

} // namespace Engine
