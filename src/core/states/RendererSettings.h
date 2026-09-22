#pragma once

#include "glad/glad.h"
#include <glm/glm.hpp>

namespace Engine {

// TODO: temp, move
enum RenderChoice { RASTERIZER, PATH_TRACER };

struct RendererSettings {
  public:
    // OpenGL version
    static inline GLint openGlMajorVersion = 0;
    static inline GLint openGlMinorVersion = 0;

    // Compatibility flags
    static inline bool systemComputeShaderCompatability = false;

    // Renderer choice
    // TODO: temp, figure out how to default this
    static inline RenderChoice currentRenderChoice = RenderChoice::RASTERIZER;

    // Rasterizer - Shadow mapping configs
    static inline GLint shadowWidth = 1024;
    static inline GLint shadowHeight = 1024;
    static inline float shadowNear = 0.5f;
    static inline float shadowFar = 100.0f;

    // Render settings (stylizing)
    static inline float exposure = 0.5;

    static inline bool toneMap = true;
    static inline bool srgb = true;

    static inline bool fog = false;
    static inline glm::vec3 fogColor = {0.5, 0.5, 0.5};
    static inline float fogDensity = 0.03;

    static inline bool vignette = false;
    static inline float vignetteRadius = 2.5;
    static inline float vignetteSoftness = 0.9;
};

} // namespace Engine
