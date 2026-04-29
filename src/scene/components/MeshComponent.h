#pragma once
#include <glad/glad.h>

namespace Engine {

// Mesh component struct
struct MeshComponent {

    // Opengl buffers / arrays
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    // Required information in glDrawElements
    unsigned int IndexCount = 0;
};

} // namespace Engine
