#pragma once

#include "IRenderer.h"

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "../scene/Camera.h"
#include "Shader.h"
#include "buffers/BufferManager.h"
#include "buffers/GPUBuffer.h"

#include <glad/glad.h>

namespace Engine {

struct RasterDrawCommand : DrawCommand {
    GLuint vao;
    GLuint indexCount;
    glm::mat4 modelMatrix;
};

class Rasterizer : public IRenderer {
  public:
    inline Rasterizer(EngineContext &engineContext)
        : engineContext(engineContext){};

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
    void tempGenDefaultTextures();
    void tempGenShadowCubemap(EngineState &state);

  private:
    EngineContext &engineContext;

    int frameIndex = 0;

    // Render size information
    int currentWidth = 0;
    int currentHeight = 0;

    GLuint defaultWhiteTexture = 0;
    GLuint defaultNormalTexture = 0;

    GLuint shadowFBO = 0;
    GLuint shadowCubeMap = 0;
    GLuint shadowDepthRBO = 0;

    std::vector<RenderPacket> renderPackets;

    CameraData cameraData;
    BufferHandle cameraUBO;

    glm::vec3 activeLightPos;
    glm::mat4 shadowProjMatrix;
};

} // namespace Engine
