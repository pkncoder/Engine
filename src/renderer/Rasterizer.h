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

    void init(EngineState &state);
    void render(EngineState &state) override;
    void resize(const int newWidth, const int newHeight) override;
    void shutdown() override;

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
    GLuint shadowCubeMap = 0; // replaces shadowDepthTexture
    GLuint shadowDepthRBO =
        0; // depth renderbuffer for shadow pass depth testing
};

} // namespace Engine
