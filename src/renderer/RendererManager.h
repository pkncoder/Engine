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
    static void init(RendererState &state);
    static void shutdown();

    static inline IRenderer *getActiveRenderer() { return activeRenderer; }

    static void swapActiveRenderer(RenderChoice choice);

    static void render(const Window &window, class Scene &scene,
                       const Camera &camera);

  private:
    static inline std::unique_ptr<Rasterizer> rasterizer = nullptr;
    static inline std::unique_ptr<PathTracer> pathTracer = nullptr;

    static inline IRenderer *activeRenderer = nullptr;
};

} // namespace Engine
