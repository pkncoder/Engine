#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "BufferManager.h"
#include "GPUStructs.h"
#include "IRenderer.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

class PathTracer : public IRenderer {
  public:
    // Init and shutdown
    void init() override;
    void shutdown() override;

    // Rendering and resizing windows
    void render(const Camera &camera, Scene &activeScene,
                float aspectRatio) override;
    void resize(int newWidth, int newHeight) override;

    // Getter for the output texture
    // GLuint getOutputTexture() const { return outputTexture; }

    // Bliting the output texture onto the framebuffer
    void present(int width, int height) override;

  private:
    // Scene modifications & building
    void flattenScene(Scene &activeScene);
    void rebuildGeometryLookupTable(Scene &activeScene);

    // Render steps
    void bindComputePipeline(const Camera &camera);
    void dispatchCompute();

  private:
    // Shader w/ the program + compute shader code
    Shader computeShader;

    // Saved image dimentions
    int currentWidth = 0;
    int currentHeight = 0;

    // Number of rendered frames (initialized to 1)
    int frameCount = 1;

    // Texture the compute shader writes to
    GLuint outputTexture = 0;

    // Framebuffer that the texture will be blited onto
    GLuint presentFBO = 0;

    // SSBOs that get sent over to the GPU
    PersistentBuffer meshEntryBuffer; // Mesh information
    PersistentBuffer vertexBuffer;    // Vertex pool
    PersistentBuffer indexBuffer;     // Indicie pool
    PersistentBuffer instanceBuffer;  // Buffer for each per-instance info
    PersistentBuffer materialBuffer;  // Material information buffer

    // Geometry state tracking
    bool geometryDirty = true;
    std::unordered_map<std::string, uint32_t> instanceLookupTable;

    // Maximum instances we allocate memory for up front + the instances vector
    const size_t MAX_INSTANCES = 10000;
    size_t instanceCount = 0;

    // Vectors made for scene flattening
    std::vector<GPUInstance> instances;
    std::vector<GPUMaterial> materialList;
};

} // namespace Engine
