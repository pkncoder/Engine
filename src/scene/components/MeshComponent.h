#pragma once
#include <glad/glad.h>

namespace Engine {
struct MeshComponent {
    GLuint VAO = 0; // Rasterizer needs this
    GLuint VBO = 0;
    GLuint EBO = 0;
    unsigned int IndexCount = 0; // Needed for glDrawElements

    // Future Path Tracer Data:
    // GLuint SSBO_Offset = 0;
    // GLuint MaterialID = 0;
};
} // namespace Engine
