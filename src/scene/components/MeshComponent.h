#pragma once

#include <glad/glad.h>
#include <string>

namespace Engine {

// Mesh component struct
struct MeshComponent {

    // --- Path Tracer Data ---
    // The unique identifier used to look up this mesh's geometry in the Atlas
    std::string assetID = "";

    // --- Rasterizer Fallback Data (Optional) ---
    // Note: If you are 100% committing to the Path Tracer, you can actually
    // delete these four variables entirely to save memory! The Atlas handles
    // it. However, keeping them allows you to toggle between Path Traced and
    // Rasterized rendering modes instantly.
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    unsigned int indexCount = 0;
};

} // namespace Engine
