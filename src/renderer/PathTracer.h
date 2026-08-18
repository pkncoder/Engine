#pragma once

#include "IRenderer.h"

#include "../core/EngineContext.h"
#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "GPUStructs.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

// Output texture struct
struct RenderTarget {
  public:
    // Name
    std::string name;

    // Binding values
    GLuint id = 0;
    GLuint bindingIndex = 0;

    // Texture color format
    GLenum format = GL_RGBA32F;
};

// Structure to define a pass
struct ShaderPass {
  public:
    // Shader / program to run
    Shader shader;

    // Name
    std::string name;

    // Compute shader dispatch size
    // TODO: Constant?
    glm::ivec3 workgroupSize{8, 8, 1};

    // TODO: dispatch size overide

    // To skip the pass - defaults to true
    bool enabled = true;
};

class PathTracer : public IRenderer {
  public:
    // Initializer just to inject the engine context
    inline PathTracer(EngineContext &engineContext)
        : engineContext(engineContext){};

    // Init & shutdown
    void init();
    void shutdown() override;

    // Rendering & render management
    void render(EngineState &state) override;
    void resize(const int newWidth, const int newHeight) override;

    // Display target use
    void setDisplayTarget(const std::string &name);
    void present(const int width, const int height) const override;

    // Dynamic resource management

    // Buffers
    void addStorageBuffer(const std::string &name, GLuint bindingIndex,
                          size_t elementSize, size_t initialElementCount);
    // Render target / display target
    void addRenderTarget(const std::string &name, GLuint bindingIndex,
                         GLenum format = GL_RGBA32F);
    // Shader pass
    void addShaderNode(const std::string &name, const char *computeShaderPath,
                       const bool enabled = true);

  private:
    // Render target management
    void allocateRenderTarget(RenderTarget &target);
    void bindRenderTarget(RenderTarget &target);

    // Buffer management
    void updateBuffer(const std::string &name, const void *data,
                      size_t elementCount);

    // Scene management
    void flattenScene();
    void rebuildGeometryLookupTable(Scene &activeScene);

    // Global uniforms
    void bindGlobalUniforms(Shader &shader, const Camera &camera) const;

    // Shader pass mangment
    void dispatchShaderPass(const ShaderNode &pass) const;

  private:
    // Engine context - Injected
    EngineContext &engineContext;

    // Render size
    int currentWidth = 0;
    int currentHeight = 0;

    // Name of the renderTarget to be presented + the framebuffer
    std::string currentRenderTarget;
    GLuint presentFBO = 0;

    // State tracking
    int frameCount =
        0; // TODO: Use the "Timer" service & add an "accumulatedFrames"

    // Dynamic resource registries
    std::unordered_map<std::string, SSBO> storageBuffers;
    std::unordered_map<std::string, RenderTarget> renderTargets;
    std::vector<ShaderNode> shaderPasses;

    // Instance data cache
    std::vector<GPUInstance> instances;
    std::unordered_map<std::string, uint32_t> instanceLookupTable;

    // Material data cache
    std::vector<GPUMaterial> materialList;
};

} // namespace Engine
