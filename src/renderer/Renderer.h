#pragma once

#include "../core/states/EngineState.h"
#include "../resources/CPUStructs.h"
#include "buffers/GPUBuffer.h"
#include "shaders/IProgram.h"

#include <memory>

namespace Engine {

using RenderTargetHandle = uint32_t;
constexpr RenderTargetHandle INVALID_RENDER_TARGET = 0;

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

struct DrawCommand {};

struct RenderPacket {
    AssetHandle meshHandle;
    AssetHandle materialHandle;

    glm::mat4 modelMatrix;
};

struct RenderLayer {
    GLuint fbo = 0;

    size_t renderWidth;
    size_t renderHeight;

    // TODO: Temp; this will need to be moved into individual layers once the
    // tree is defined
    bool isShadowPass = false; // A flag to help dispatch know what to do
    std::vector<glm::mat4> shadowTransforms;

    std::vector<std::shared_ptr<DrawCommand>> commands;
};

// Structure to define a pass
struct ShaderNode {
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
    IProgram program;
    bool enabled = true;

    std::vector<AssetHandle> textureInputs;
    std::vector<RenderTargetHandle> renderTargets;
    std::vector<BufferHandle> bufferInputs;

    std::vector<RenderLayer> renderSteps;
};

class Renderer {
  public:
    Renderer() = default;
    ~Renderer() = default; // Deconstructor

    // --- Lifecycle ---

    void shutdown();
    void resize(const uint32_t width, const uint32_t height);

    // --- Render & Shader Passes ---

    // Render target handling
    RenderTargetHandle addRenderTarget(const std::string &name,
                                       const GLuint bindingIndex,
                                       const GLenum format = GL_RGBA32F);
    void setDisplayTarget(const RenderTargetHandle handle);

    // Lookups
    RenderTarget *getRenderTarget(const RenderTargetHandle handle);
    RenderTarget *getRenderTargetByName(const std::string &name);

    void execute(EngineState &state);

  private:
    // Methods for setting up render targets
    void allocateRenderTarget(RenderTarget &target) const;
    void bindRenderTarget(RenderTarget &target) const;

  private:
    // Tracked render width & height
    uint32_t currentWidth;
    uint32_t currentHeight;

    RenderTargetHandle currentRenderTarget;

    // Registry for the render targets + another map for str -> handle
    std::unordered_map<RenderTargetHandle, RenderTarget> renderTargets;
    std::unordered_map<std::string, RenderTargetHandle> renderTargetNameMap;

    // Shader passes
    std::vector<ShaderNode> shaderNodeTree;
};

}; // namespace Engine
