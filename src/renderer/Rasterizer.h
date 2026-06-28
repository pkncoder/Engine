#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "IRenderer.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Engine {

class Rasterizer : public IRenderer {
  public:
    void init() override;
    void render(const Camera &camera, Scene &scene, float aspectRatio) override;
    void resize(const int newWidth, const int newHeight) override;
    void shutdown() override;

  private:
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

    static constexpr int SHADOW_WIDTH = 1024;
    static constexpr int SHADOW_HEIGHT = 1024;
    static constexpr float SHADOW_NEAR = 0.5f;
    static constexpr float SHADOW_FAR = 100.0f;
};

} // namespace Engine
