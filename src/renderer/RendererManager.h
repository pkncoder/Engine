#include "IRenderer.h"

#include "PathTracer.h"
#include "Rasterizer.h"

#include <c++/v1/__config>
#include <cstdlib>
#include <memory>

namespace Engine {

enum RenderChoice { RASTERIZER, PATH_TRACER };

class RendererManager {
  public:
    static void init();

    static inline IRenderer *getActiveRenderer() { return activeRenderer; }
    static inline const RenderChoice getRenderChoice() {
        return currentRenderChoice;
    }
    static inline const bool getComputeShaderCompatibility() {
        return systemComputeShaderCompatability;
    }

    static void swapActiveRenderer(RenderChoice choice);

  private:
    static inline GLint openGlMajorVersion = 0;
    static inline GLint openGlMinorVersion = 0;

    static inline bool systemComputeShaderCompatability = false;

    static inline std::unique_ptr<Rasterizer> rasterizer = nullptr;
    static inline std::unique_ptr<PathTracer> pathTracer = nullptr;

    static inline RenderChoice currentRenderChoice;

    static inline IRenderer *activeRenderer = nullptr;
};

} // namespace Engine
