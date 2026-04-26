#pragma once

#include "../scene/camera.h"
#include "IRenderer.h"
#include "glad/glad.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>

class Rasterizer : IRenderer {
  public:
    uint32_t m_TestVAO; // TODO: temp

    void Init(); // Init the renderer

    void Render(const Camera &camera, float aspectRatio); // Render frame

    void Shutdown(); // Shut down the renderer

  private:
    // Shader code
    Shader m_Shader;
};
