#pragma once

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "Renderer.h"

#include <unordered_map>

namespace Engine {

using RendererHandle = uint32_t;

class RendererManager {
  public:
    // Init & Shutdown
    RendererManager(EngineContext &engineContext, EngineState &state);
    void shutdown();

    // Active renderer modifications
    inline Renderer *getActiveRenderer() const { return activeRenderer; }
    void swapActiveRenderer(const RendererHandle handle);

    RendererHandle createNewRenderer();
    Renderer *getRenderer(RendererHandle handle);
    void destroyRenderer(const RendererHandle handle);

    // Renderer functions
    void render(EngineState &state);
    void resize(const int newWidth, const int newHeight);

  private:
    // Injected engine context
    EngineContext &engineContext;

    std::unordered_map<RendererHandle, std::shared_ptr<Renderer>>
        rendererRegistry;

    // Active renderer choice (ptr to the uniques)
    Renderer *activeRenderer = nullptr;
};

} // namespace Engine
