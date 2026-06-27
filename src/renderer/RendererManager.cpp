#include "RendererManager.h"

#include "../services/Logger.h"
#include "../services/Timer.h"

namespace Engine {

// TODO: Add a param for what to default the active renderer to
void RendererManager::init() {

    // Rasterizer inizialization
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();

    // Check for OpenGL compatibility
    glGetIntegerv(GL_MAJOR_VERSION, &openGlMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &openGlMinorVersion);

    // Check for compute shader compatibility
    if (openGlMajorVersion > 4 ||
        (openGlMajorVersion == 4 && openGlMinorVersion >= 6)) {

        // Set compute shader compatible
        systemComputeShaderCompatability = true;

        // Path tracer inizialization
        pathTracer = std::make_unique<PathTracer>();
        pathTracer->init();
    } else {
        Logger::warn("RENDERER", "Path Tracer not supported on this system.");
    }

    // Set the active renderer
    activeRenderer = rasterizer.get();
    currentRenderChoice = RenderChoice::RASTERIZER;

    Logger::info("RENDERER", "Renderer Manager initialized.");
}

void RendererManager::shutdown() {
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
        currentRenderChoice = RenderChoice::RASTERIZER;
        Logger::info("RENDERER", "Swapped to Rasterizer.");
        break;
    case RenderChoice::PATH_TRACER:
        // Make sure to check for compatibility
        if (systemComputeShaderCompatability) {
            activeRenderer = pathTracer.get();
            currentRenderChoice = RenderChoice::PATH_TRACER;
            Logger::info("RENDERER", "Swapped to Path Tracer.");
        } else {
            Logger::error("RENDERER",
                          "Path Tracer is not supported on this system.");
        }
        break;
    }
}

void RendererManager::render(const Window &window, class Scene &scene,
                             const Camera &camera) {
    // Render the scene
    START_PROFILE("Render"); // Start timer for render
    activeRenderer->render(camera, scene, window.getAspectRatio());
    END_PROFILE("Render"); // End Timer for render

    int width, height;
    window.getSize(width, height);
    activeRenderer->resize(width, height);

    // 2. Present the compute texture to the main window
    activeRenderer->present(width, height);
}

} // namespace Engine
