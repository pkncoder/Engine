#pragma once

#include "../core/states/EngineState.h"
#include "../resources/CPUStructs.h"
#include "Shader.h"
#include "buffers/GPUBuffer.h"

#include <memory>

namespace Engine {

using RenderTargetHandle = uint32_t;
constexpr RenderTargetHandle INVALID_RENDER_TARGET = 0;

struct DrawCommand {};

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
    GLuint fbo = 0;

    bool enabled = true;

    std::vector<AssetHandle> textureInputs;
    std::vector<RenderTargetHandle> renderTargets;
    std::vector<BufferHandle> bufferInputs;

    std::vector<std::shared_ptr<DrawCommand>> commands;
};

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    // --- Lifecycle ---

    virtual void init(EngineState &state) = 0;
    virtual void shutdown() = 0;
    virtual void resize(const uint32_t width, const uint32_t height) = 0;

    // --- Render & Shader Passes ---

    // Render target handling
    virtual RenderTargetHandle
    addRenderTarget(const std::string &name, const GLuint bindingIndex,
                    const GLenum format = GL_RGBA32F);
    virtual void setDisplayTarget(const RenderTargetHandle handle);

    // Shader pass overloads
    virtual ShaderPass &
    addShaderPass(const std::string &name, const char *vertexShaderPath,
                  const char *fragmentShaderPath,
                  const bool enabled = true); // Vert & Frag shaders
    virtual ShaderPass &
    addShaderPass(const std::string &name, const char *computeShaderPath,
                  const bool enabled = true); // Compute shaders

    // Lookups
    RenderTarget *getRenderTarget(const RenderTargetHandle handle);
    RenderTarget *getRenderTargetByName(const std::string &name);

    // --- The Frame Pipeline ---

    // Reseting and setting up for frame
    virtual void beginFrame(EngineState &state) = 0;

    // Getting and formatting data
    virtual void extract(EngineState &state) = 0;
    virtual void prepare(EngineState &state) = 0;

    // Sending shader passes
    virtual void dispatch(EngineState &state) = 0;
    virtual void postProcess(EngineState &state) = 0;

    // Presenting / blitting to FOB
    virtual void present(EngineState &state) = 0;

  protected:
    // Methods for setting up render targets
    virtual void allocateRenderTarget(RenderTarget &target) const;
    virtual void bindRenderTarget(RenderTarget &target) const;

  protected:
    // Tracked render width & height
    uint32_t currentWidth;
    uint32_t currentHeight;

    RenderTargetHandle currentRenderTarget;

    // Registry for the render targets + another map for str -> handle
    std::unordered_map<RenderTargetHandle, RenderTarget> renderTargets;
    std::unordered_map<std::string, RenderTargetHandle> renderTargetNameMap;

    // Shader passes
    std::vector<ShaderPass> shaderPasses;
};

}; // namespace Engine
