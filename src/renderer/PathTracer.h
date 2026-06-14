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

    glm::ivec3 workgroupSize{8, 8, 1};

    // TODO: dispatch size overide?
    bool enabled = true;
};

class PathTracer : public IRenderer {
  public:
    void init() override;
    void shutdown() override;

    void render(const Camera &camera, Scene &activeScene,
                float aspectRatio) override;
    void resize(int newWidth, int newHeight) override;
    void present(int width, int height) override;

    // --- NEW: Dynamic Resource Management ---
    void addStorageBuffer(const std::string &name, GLuint bindingIndex,
                          size_t elementSize, size_t initialElementCount);
    void addRenderTarget(const std::string &name, GLuint bindingIndex,
                         GLenum format = GL_RGBA32F);
    void addShaderPass(const std::string &name, const char *computeShaderPath);

    // Choose which texture blits to the screen
    void setDisplayTarget(const std::string &name);

  private:
    // Core steps separated for multi-pass versatility
    void flattenScene(Scene &activeScene);
    void rebuildGeometryLookupTable(Scene &activeScene);
    void updateBuffer(const std::string &name, const void *data,
                      size_t elementCount);

    // Shader pass mangment
    void dispatchShaderPass(const ShaderPass &pass);

    void allocateRenderTarget(RenderTarget &target);
    void bindRenderTarget(RenderTarget &target);

    // Uniforms
    void bindGlobalUniforms(Shader &shader, const Camera &camera);

  private:
    // Sizing and state
    int currentWidth = 0;
    int currentHeight = 0;
    int frameCount = 0;
    GLuint presentFBO = 0;

    std::unordered_map<std::string, PersistentBuffer> storageBuffers;
    std::unordered_map<std::string, RenderTarget>
        renderTargets; // TODO: string name
    std::vector<ShaderPass> shaderPasses;

    std::string currentDisplayTarget; // TODO: string?

    // Instance data cache
    std::vector<GPUInstance> instances;
    std::unordered_map<std::string, uint32_t> instanceLookupTable;
    uint32_t instanceCount = 0;
    bool geometryDirty = true;

    // Material data cache
    std::vector<GPUMaterial> materialList;
};

} // namespace Engine
