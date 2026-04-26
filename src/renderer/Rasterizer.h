#pragma once

#include "../scene/Camera.h"
#include "IRenderer.h"
#include "Shader.h"

namespace Engine {

class Rasterizer : IRenderer {
  public:
    uint32_t testVAO; // TODO: temp

    void init(); // Init the renderer

    void render(const Camera &camera, float aspectRatio); // Render frame

    void shutdown(); // Shut down the renderer

  private:
    // Shader code
    Shader shader;
};

} // namespace Engine
