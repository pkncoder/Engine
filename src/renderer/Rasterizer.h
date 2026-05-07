#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "IRenderer.h"
#include "Shader.h"

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
};

} // namespace Engine
