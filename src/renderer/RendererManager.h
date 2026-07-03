#include "../core/EngineContext.h"
#include "../core/Window.h"
#include "../core/states/RendererState.h"
#include "../scene/Camera.h"
#include "IRenderer.h"
#include "PathTracer.h"
#include "Rasterizer.h"

#include <cstdlib>
#include <memory>

namespace Engine {

class RendererManager {
  public:
    RendererManager(EngineContext &engineContext, RendererState &state);
    ~RendererManager();

    IRenderer *getActiveRenderer() { return activeRenderer; }

    void swapActiveRenderer(RenderChoice choice);

    void render(const Window &window, const Camera &camera);

  private:
    EngineContext &engineContext;

    std::unique_ptr<Rasterizer> rasterizer = nullptr;
    std::unique_ptr<PathTracer> pathTracer = nullptr;

    IRenderer *activeRenderer = nullptr;
};

} // namespace Engine
