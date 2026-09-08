#pragma once

#include "../core/states/EngineState.h"
#include "renderGraph/RenderGraph.h"

namespace Engine {

class Renderer {
  public:
    Renderer() = default;
    ~Renderer() = default; // Deconstructor

    // --- Lifecycle ---

    void shutdown();
    void resize(const uint32_t width, const uint32_t height);

    void execute(EngineState &state);

  private:
    // Tracked render width & height
    uint32_t presentWidth;
    uint32_t presentHeight;

    RenderGraph RenderGraph;
};

}; // namespace Engine
