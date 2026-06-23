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

// Structure to define dynamic output textures
struct RenderTarget {
    std::string name;
    GLuint id = 0;
    GLuint bindingIndex = 0;
    GLenum format = GL_RGBA32F;
};

// Structure to define a dynamic pass
struct ShaderPass {
    std::string name;
    Shader shader;

    // TODO: Constant
    glm::ivec3 workgroupSize{8, 8, 1};

    // TODO: dispatch size overide
    bool enabled = true;
};

class PathTracer : public IRenderer {
  public:
    // Init & shutdown
    void init() override;
    void shutdown() override;

    // Rendering & render management
    void render(const Camera &camera, Scene &activeScene,
                float aspectRatio) override;
    void resize(int newWidth, int newHeight) override;

    // Presenting rendering target
    void setDisplayTarget(const std::string &name);
    void present(int width, int height) override;

    // Dynamic resource management
    void addStorageBuffer(const std::string &name, GLuint bindingIndex,
                          size_t elementSize, size_t initialElementCount);
    void addRenderTarget(const std::string &name, GLuint bindingIndex,
                         GLenum format = GL_RGBA32F);
    void addShaderPass(const std::string &name, const char *computeShaderPath,
                       const bool enabled = true);

  private:
    // Render target management
    void allocateRenderTarget(RenderTarget &target);
    void bindRenderTarget(RenderTarget &target);

    // Buffer management
    void updateBuffer(const std::string &name, const void *data,
                      size_t elementCount);

    // Scene management
    void flattenScene(Scene &activeScene);
    void rebuildGeometryLookupTable(Scene &activeScene);

    // Uniforms
    void bindGlobalUniforms(Shader &shader, const Camera &camera);

    // Shader pass mangment
    void dispatchShaderPass(const ShaderPass &pass);

  private:
    // Render size information
    int currentWidth = 0;
    int currentHeight = 0;

    // State tracking
    int frameCount = 0; // TODO: Use the "Timer" service

    // Dynamic resource information
    std::unordered_map<std::string, PersistentBuffer> storageBuffers;
    std::unordered_map<std::string, RenderTarget> renderTargets;
    std::vector<ShaderPass> shaderPasses;

    // Name of the renderTarget to be presented + the framebuffer
    std::string currentRenderTarget;
    GLuint presentFBO = 0;

    // Instance data cache
    std::vector<GPUInstance> instances;
    const size_t MAX_INSTANCES =
        10000; // TODO: renderer settings maybe? TODO: Constant
    std::unordered_map<std::string, uint32_t> instanceLookupTable;
    bool geometryDirty = true; // TODO: move tracking elsewhere, this honestly
                               // shouldn'tbe PathTracer's job

    // Material data cache
    std::vector<GPUMaterial> materialList;
};

} // namespace Engine
