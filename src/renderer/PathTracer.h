#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "BufferManager.h"
#include "IRenderer.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace Engine {

// --- GPU Data Structures (Must match GLSL std430 exactly) ---

struct alignas(16) GPUMeshEntry {
    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t indexCount;
    uint32_t padding;
};

struct alignas(16) GPUVertex {
    glm::vec4 position;  // w unused
    glm::vec4 normal;    // w unused
    glm::vec4 texCoords; // z, w unused
};

struct alignas(16) GPUInstance {
    glm::mat4 transform;
    glm::mat4 invTransform;
    uint32_t meshIndex;
    uint32_t padding[3]; // Pad to 16 bytes
};

// ------------------------------------------------------------

class PathTracer : IRenderer {
  public:
    // Init and shutdown
    void init();
    void shutdown();

    // Rendering and resizing windows
    void render(const Camera &camera, Scene &activeScene, float aspectRatio);
    void resize(int newWidth, int newHeight);

    // Getter for the output texture
    // GLuint getOutputTexture() const { return outputTexture; }

    // Bliting the output texture onto the framebuffer
    void presentOutputTextureToFramebuffer(int width, int height) const;

  private:
    // Scene modifications & building
    void flattenScene(Scene &activeScene);
    void rebuildGeometryLookupTable(Scene &activeScene);

  private:
    // Shader w/ the program + compute shader code
    Shader computeShader;

    // Saved image dimentions
    int currentWidth = 0;
    int currentHeight = 0;

    // Texture the compute shader writes to
    GLuint outputTexture = 0;

    // Framebuffer that the texture will be blited onto
    GLuint presentFBO = 0;

    // SSBOs that get sent over to the GPU
    PersistentBuffer meshEntryBuffer; // Mesh information
    PersistentBuffer vertexBuffer;    // Vertex pool
    PersistentBuffer indexBuffer;     // Indicie pool
    PersistentBuffer instanceBuffer;  // Buffer for each per-instance info

    // Geometry state tracking
    bool geometryDirty = true;
    std::unordered_map<std::string, uint32_t> instanceLookupTable;

    // Maximum instances we allocate memory for up front
    const size_t MAX_INSTANCES = 10000;
};

} // namespace Engine
