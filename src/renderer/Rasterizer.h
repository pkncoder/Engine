#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "IRenderer.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Engine {

class Rasterizer : public IRenderer {
  public:
    void init() override; // Init the renderer

    void render(const Camera &camera, Scene &scene,
                float aspectRatio) override; // Render frame

    void shutdown() override; // Shut down the renderer

  private:
    // Shader code
    Shader shader;

    // Default textures
    // TODO: temp
    GLuint defaultWhiteTexture = 0;
    GLuint defaultNormalTexture = 0;
};

} // namespace Engine
