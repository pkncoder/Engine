#include "RendererManager.h"

#include "../services/Logger.h"
#include "../services/Timer.h"

namespace Engine {

RendererManager::RendererManager(EngineContext &engineContext,
                                 EngineState &state)
    : engineContext(engineContext) {

    // Rasterizer inizialization
    rasterizer = std::make_unique<Rasterizer>(engineContext);
    rasterizer->init(state);

    // Check for OpenGL compatibility
    glGetIntegerv(GL_MAJOR_VERSION,
                  &state.renderer.settings.openGlMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION,
                  &state.renderer.settings.openGlMinorVersion);

    // Check for compute shader compatibility
    if (state.renderer.settings.openGlMajorVersion > 4 ||
        (state.renderer.settings.openGlMajorVersion == 4 &&
         state.renderer.settings.openGlMinorVersion >= 6)) {

        // Set compute shader compatible
        state.renderer.settings.systemComputeShaderCompatability = true;

        // Path tracer inizialization
        pathTracer = std::make_unique<PathTracer>(engineContext);
        pathTracer->init();
    } else {
        Logger::warn("RENDERER", "Path Tracer not supported on this system.");
    }

    // Set the active renderer
    activeRenderer = rasterizer.get();
    state.renderer.settings.currentRenderChoice = RenderChoice::RASTERIZER;

    Logger::info("RENDERER", "Renderer Manager initialized.");
}

RendererManager::~RendererManager() {
    activeRenderer = nullptr;

    if (rasterizer)
        rasterizer->shutdown();
    if (pathTracer)
        pathTracer->shutdown();
}

void RendererManager::swapActiveRenderer(const RenderChoice choice) {
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
void RendererManager::render(EngineState &state) {
    // Render the scene
    START_PROFILE("Render"); // Start timer for render
    activeRenderer->render(state);
    END_PROFILE("Render"); // End Timer for render
    //
    // 2. Present the compute texture to the main window
    activeRenderer->present(state.window.width, state.window.height);
}

void RendererManager::resize(const int newWidth, const int newHeight) {
    activeRenderer->resize(newWidth, newHeight);
}

} // namespace Engine
