#include "RendererManager.h"

#include "../services/Logger.h"

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
    }

    // Set the active renderer
    activeRenderer = rasterizer.get();
    currentRenderChoice = RenderChoice::RASTERIZER;

    Logger::info("RENDERER", "Renderer Manager initialized.");
}

void RendererManager::swapActiveRenderer(RenderChoice choice) {
    switch (choice) {
    case RenderChoice::RASTERIZER:
        activeRenderer = rasterizer.get();
        currentRenderChoice = RenderChoice::RASTERIZER;
    case RenderChoice::PATH_TRACER:
        // Make sure to check for compatibility
        if (systemComputeShaderCompatability) {
            activeRenderer = pathTracer.get();
            currentRenderChoice = RenderChoice::PATH_TRACER;
        } else {
            Logger::error("RENDERER",
                          "Path Tracer is not supported on this system.");
        }
    }
}

} // namespace Engine
