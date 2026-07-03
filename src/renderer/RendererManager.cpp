#include "RendererManager.h"

#include "../services/Logger.h"
#include "../services/Timer.h"

namespace Engine {

RendererManager::RendererManager(EngineContext &engineContext,
                                 RendererState &state)
    : engineContext(engineContext) {

    // Rasterizer inizialization
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();

    // Check for OpenGL compatibility
    glGetIntegerv(GL_MAJOR_VERSION, &state.settings.openGlMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &state.settings.openGlMinorVersion);

    // Check for compute shader compatibility
    if (state.settings.openGlMajorVersion > 4 ||
        (state.settings.openGlMajorVersion == 4 &&
         state.settings.openGlMinorVersion >= 6)) {

        // Set compute shader compatible
        state.settings.systemComputeShaderCompatability = true;

        // Path tracer inizialization
        pathTracer = std::make_unique<PathTracer>();
        pathTracer->init();
    } else {
        Logger::warn("RENDERER", "Path Tracer not supported on this system.");
    }

    // Set the active renderer
    activeRenderer = rasterizer.get();
    state.settings.currentRenderChoice = RenderChoice::RASTERIZER;

    Logger::info("RENDERER", "Renderer Manager initialized.");
}

RendererManager::~RendererManager() {
    activeRenderer = nullptr;

    if (rasterizer)
        rasterizer->shutdown();
    if (pathTracer)
        pathTracer->shutdown();
}

void RendererManager::swapActiveRenderer(RenderChoice choice) {
    switch (choice) {
    case RenderChoice::RASTERIZER:
        activeRenderer = rasterizer.get();
        Logger::info("RENDERER", "Swapped to Rasterizer.");
        break;
    case RenderChoice::PATH_TRACER:
        // Make sure to check for compatibility
        activeRenderer = pathTracer.get();
        Logger::info("RENDERER", "Swapped to Path Tracer.");
        break;
    }
}

// TODO: replace with context
void RendererManager::render(const Window &window, const Camera &camera) {
    // Render the scene
    START_PROFILE("Render"); // Start timer for render
    activeRenderer->render(camera, engineContext.getScene(),
                           window.getAspectRatio());
    END_PROFILE("Render"); // End Timer for render

    int width, height;
    window.getSize(width, height);
    activeRenderer->resize(width, height);

    // 2. Present the compute texture to the main window
    activeRenderer->present(width, height);
}

} // namespace Engine
