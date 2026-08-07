#pragma once

#include "IRenderer.h"

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Engine {

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
    EngineContext &engineContext;

    // Render size information
    int currentWidth = 0;
    int currentHeight = 0;

    Shader shader;
    Shader shadowShader;

    GLuint defaultWhiteTexture = 0;
    GLuint defaultNormalTexture = 0;

    GLuint shadowFBO = 0;
    GLuint shadowCubeMap = 0;
    GLuint shadowDepthRBO = 0;

    std::vector<RenderPacket> m_RenderPackets;
    glm::vec3 m_ActiveLightPos;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjMatrix;
    glm::mat4 m_ShadowProjMatrix;
};

} // namespace Engine
