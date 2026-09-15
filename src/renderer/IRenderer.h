#pragma once

#include "../core/states/EngineState.h"
#include "../resources/CPUStructs.h"

#include <vector>

namespace Engine {

struct PointLightData {
    glm::vec3 position = {0, 0, 0};
    float padding = 0;
    glm::vec3 emissive = {0, 0, 0};
    float intensity = 0;
};

struct LightUBOData {
    PointLightData lights[4] = {}; // TODO: static var
    float lightCount = 0;
    glm::vec3 padding = {0, 0, 0};
};

using RenderTargetHandle = uint32_t;
constexpr RenderTargetHandle INVALID_RENDER_TARGET = 0;

struct RenderTarget {
    std::string name;
    RenderTargetHandle handle = INVALID_RENDER_TARGET;

    GLuint fbo = 0;                 // The actual framebuffer
    std::vector<GLuint> textureIDs; // Multiple color attachments (MRTs)
    GLuint depthTextureID = 0;      // Optional depth attachment

    std::vector<GLenum> formats; // Formats for each color attachment
};

struct RasterDrawCommand {
    GLuint vao;
    GLuint indexCount;
    glm::mat4 modelMatrix;

    glm::vec3 albedo;
    glm::vec3 emissive;
    float roughness;
    float metallic;
    bool isBumpMap; // Store the logic here!

    /*
     * Albedo
     * Emmissive
     * Alpha
     * ARM
     * Specular
     */
    int textures[8] = {0};
};
struct RenderPacket {
    AssetHandle meshHandle;
    AssetHandle materialHandle;

    glm::mat4 modelMatrix;
};

// struct RenderLayer {
//     GLuint fbo = 0;
//
//     size_t renderWidth;
//     size_t renderHeight;
//
//     // TODO: Temp; this will need to be moved into individual layers once the
//     // tree is defined
//     bool isShadowPass = false; // A flag to help dispatch know what to do
//     std::vector<glm::mat4> shadowTransforms;
//
//     std::vector<RasterDrawCommand> commands;
// };
//
// // Structure to define a pass
// struct ShaderNode {
//   public:
//     inline void addTextureInput(AssetHandle textureHandle) {
//         textureInputs.push_back(textureHandle);
//     }
//
//     inline void addRenderTarget(RenderTargetHandle renderTargetHandle) {
//         renderTargets.push_back(renderTargetHandle);
//     }
//
//     inline void addBufferInput(BufferHandle bufferHandle) {
//         bufferInputs.push_back(bufferHandle);
//     }
//
//   public:
//     // Name
//     std::string name;
//
//     // Shader / program to run
//     IProgram program;
//     bool enabled = true;
//
//     std::vector<AssetHandle> textureInputs;
//     std::vector<RenderTargetHandle> renderTargets;
//     std::vector<BufferHandle> bufferInputs;
//
//     std::vector<RenderLayer> renderSteps;
// };

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
    addRenderTarget(const std::string &name, const std::vector<GLenum> &formats,
                    const bool createDepth = false);
    // virtual void setDisplayTarget(const RenderTargetHandle handle);
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
    // // Methods for setting up render targets
    // virtual void allocateRenderTarget(RenderTarget &target) const;
    // virtual void bindRenderTarget(RenderTarget &target) const;

    virtual void drawFullscreenQuad();

  protected:
    GLuint fullscreenQuadVAO = 0;

  protected:
    // Tracked render width & height
    uint32_t currentWidth;
    uint32_t currentHeight;

    RenderTargetHandle currentRenderTarget;

    // Registry for the render targets + another map for str -> handle
    std::unordered_map<RenderTargetHandle, RenderTarget> renderTargets;
    std::unordered_map<std::string, RenderTargetHandle> renderTargetNameMap;

    // // Shader passes
    // std::vector<ShaderNode> shaderNodeTree;
};

}; // namespace Engine
