#pragma once

#include "../core/states/EngineState.h"
#include "../resources/CPUStructs.h"
#include "Shader.h"
#include "buffers/GPUBuffer.h"

namespace Engine {

using RenderTargetHandle = uint32_t;
constexpr RenderTargetHandle INVALID_RENDER_TARGET = 0;

struct RenderPacket {
    AssetHandle meshHandle;
    AssetHandle materialHandle;

    glm::mat4 modelMatrix;
};

struct RenderTarget {
  public:
    // Name & Handle
    std::string name;
    RenderTargetHandle handle = INVALID_RENDER_TARGET;

    // Binding values
    GLuint id = 0;
    GLuint bindingIndex = 0;

    // Texture color format
    GLenum format = GL_RGBA32F;
};

// Structure to define a pass
struct ShaderPass {
  public:
    inline void addTextureInput(AssetHandle textureHandle) {
        textureInputs.push_back(textureHandle);
    }

    inline void addRenderTarget(RenderTargetHandle renderTargetHandle) {
        renderTargets.push_back(renderTargetHandle);
    }

    inline void addBufferInput(BufferHandle bufferHandle) {
        bufferInputs.push_back(bufferHandle);
    }

  public:
    // Name
    std::string name;

    // Shader / program to run
    Shader shader;
    bool enabled = true;

    // Compute shader dispatch size
    glm::ivec3 workgroupSize{8, 8, 1};

    std::vector<AssetHandle> textureInputs;
    std::vector<RenderTargetHandle> renderTargets;
    std::vector<BufferHandle> bufferInputs;
};

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    // --- Lifecycle ---

    virtual void init(EngineState &state) = 0;
    virtual void shutdown() = 0;
    virtual void resize(const uint32_t width, const uint32_t height) = 0;

    // --- Render & Shader Passes ---

    virtual RenderTargetHandle
    addRenderTarget(const std::string &name, const GLuint bindingIndex,
                    const GLenum format = GL_RGBA32F);
    virtual void setDisplayTarget(const RenderTargetHandle handle);

    virtual ShaderPass &addShaderPass(const std::string &name,
                                      const char *computeShaderPath,
                                      const bool enabled = true);
    virtual ShaderPass &addShaderPass(const std::string &name,
                                      const char *vertexShaderPath,
                                      const char *fragmentShaderPath,
                                      const bool enabled = true);

    // Lookups
    RenderTarget *getRenderTarget(const RenderTargetHandle handle);
    RenderTarget *getRenderTargetByName(const std::string &name);

    // --- The Frame Pipeline ---

    virtual void beginFrame(EngineState &state) = 0;

    virtual void extract(EngineState &state) = 0;
    virtual void prepare(EngineState &state) = 0;

    virtual void dispatch(EngineState &state) = 0;
    virtual void postProcess(EngineState &state) = 0;

    virtual void present(EngineState &state) = 0;

  protected:
    virtual void allocateRenderTarget(RenderTarget &target) const;
    virtual void bindRenderTarget(RenderTarget &target) const;

  protected:
    uint32_t currentWidth;
    uint32_t currentHeight;

    std::unordered_map<RenderTargetHandle, RenderTarget> renderTargets;
    std::unordered_map<std::string, RenderTargetHandle> renderTargetNameMap;

    std::vector<ShaderPass> shaderPasses;
};

}; // namespace Engine
