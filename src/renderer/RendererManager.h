#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "IRenderer.h"
#include "PathTracer.h"
#include "Rasterizer.h"

#include <memory>

namespace Engine {

class RendererManager {
  public:
    RendererManager(EngineContext &engineContext, EngineState &state);
    ~RendererManager();

    inline IRenderer *getActiveRenderer() const { return activeRenderer; }

    void swapActiveRenderer(RenderChoice choice);

    void render(EngineState &state);
    void resize(int newWidth, int newHeight);

  private:
    EngineContext &engineContext;

    std::unique_ptr<Rasterizer> rasterizer = nullptr;
    std::unique_ptr<PathTracer> pathTracer = nullptr;

    IRenderer *activeRenderer = nullptr;
};

} // namespace Engine
