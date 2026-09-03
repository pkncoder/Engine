#pragma once

#include "Renderer.h"

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "../scene/Camera.h"
#include "buffers/GPUBuffer.h"

#include <glad/glad.h>

namespace Engine {

struct RasterDrawCommand : DrawCommand {
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

class Rasterizer : public Renderer {
  public:
    inline Rasterizer(EngineContext &engineContext)
        : engineContext(engineContext) {};

    // --- Lifecycle ---
    void init(EngineState &state) override;
    void shutdown() override;
    void resize(const uint32_t newWidth, const uint32_t newHeight) override;

    // --- The Frame Pipeline ---
    void beginFrame(EngineState &state) override;
    void extract(EngineState &state) override;
    void prepare(EngineState &state) override;
    void dispatch(EngineState &state) override;
    void postProcess(EngineState &state) override;
    void present(EngineState &state) override;

  private:
    void setupDefaultTextures();
    void setupShadowFBO(EngineState &state);

    // TODO: temp
    void setupShaderNodes();
    void setupBuffers();
    void setBaseBindings();

  private:
    EngineContext &engineContext;

    int frameIndex = 0;

    GLuint defaultWhiteTexture = 0;
    GLuint defaultNormalTexture = 0;

    std::vector<RenderPacket> renderPackets;

    GLuint shadowFBO = 0;
    GLuint shadowCubeMap = 0;

    BufferHandle cameraUBO;
    BufferHandle globalSceneUBO;
    BufferHandle objectUBO;

    CameraData cameraData;
    glm::vec3 activeLightPos;
};

} // namespace Engine
