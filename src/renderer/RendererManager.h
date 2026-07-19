#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "IRenderer.h"
#include "PathTracer.h"
#include "Rasterizer.h"

#include <memory>

namespace Engine {

class RendererManager {
  public:
    // Init & Shutdown
    RendererManager(EngineContext &engineContext, EngineState &state);
    void shutdown();

    // Active renderer modifications
    inline IRenderer *getActiveRenderer() const { return activeRenderer; }
    void swapActiveRenderer(const RenderChoice choice);

    // Renderer functions
    void render(EngineState &state);
    void resize(const int newWidth, const int newHeight);

  private:
    // Injected engine context
    EngineContext &engineContext;

    // Renderers
    std::unique_ptr<Rasterizer> rasterizer = nullptr;
    std::unique_ptr<PathTracer> pathTracer = nullptr;

    // Active renderer choice (ptr to the uniques)
    IRenderer *activeRenderer = nullptr;
};

} // namespace Engine
