#pragma once

#include "glad/glad.h"

namespace Engine {

// TODO: temp, move
enum RenderChoice { RASTERIZER, PATH_TRACER };

struct RendererSettings {
  public:
    static inline GLint openGlMajorVersion = 0;
    static inline GLint openGlMinorVersion = 0;

    static inline bool systemComputeShaderCompatability = false;

    // TODO: temp, figure out how to default this
    static inline RenderChoice currentRenderChoice = RenderChoice::RASTERIZER;

    // Rasterizer - Shadow mapping configs
    static inline GLint shadowWidth = 1024;
    static inline GLint shadowHeight = 1024;
    static inline float shadowNear = 0.5f;
    static inline float shadowFar = 100.0f;
};

} // namespace Engine
