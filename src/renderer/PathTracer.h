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
    void init();
    void shutdown();

    void render(const Camera &camera, Scene &activeScene, float aspectRatio);
    void resize(int newWidth, int newHeight);

    // GLuint getOutputTexture() const { return outputTexture; }
    void presentOutputTextureToFramebuffer(int width, int height) const;

  private:
    void flattenScene(Scene &activeScene);
    void rebuildGeometryLookupTable(Scene &activeScene);

  private:
    Shader computeShader;

    int currentWidth = 0;
    int currentHeight = 0;

    GLuint outputTexture = 0;
    GLuint presentFBO = 0;

    // SSBOs using our new PersistentBuffer
    PersistentBuffer meshEntryBuffer;
    PersistentBuffer vertexBuffer;
    PersistentBuffer indexBuffer;
    PersistentBuffer instanceBuffer;

    // State Tracking
    bool geometryDirty = true;
    std::unordered_map<std::string, uint32_t> instanceLookupTable;

    // Maximum instances we allocate memory for up front
    const size_t MAX_INSTANCES = 10000;
};

} // namespace Engine
