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
    Shader m_Shader;
    uint32_t m_TestVAO;

    void Init();
    // void Shutdown();
    void Render(glm::mat4 view, glm::mat4 proj);
};
