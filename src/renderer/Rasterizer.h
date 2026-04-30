#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "IRenderer.h"
#include "Shader.h"

namespace Engine {

class Rasterizer : IRenderer {
  public:
    void init(); // Init the renderer

    void render(const Camera &camera, Scene &scene,
                float aspectRatio); // Render frame

    void shutdown(); // Shut down the renderer

  private:
    // Shader code
    Shader shader;
};

} // namespace Engine
