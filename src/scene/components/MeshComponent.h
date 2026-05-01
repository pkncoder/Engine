#pragma once

#include <glad/glad.h>

namespace Engine {

// Mesh component struct
struct MeshComponent {

    // Opengl buffers / arrays
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    // Required information in glDrawElements
    unsigned int indexCount = 0;
};

} // namespace Engine
