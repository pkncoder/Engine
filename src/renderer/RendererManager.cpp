#include "RendererManager.h"

#include "../services/Logger.h"
#include "../services/Timer.h"
#include "../services/UUID.h"
#include "GPUResourceManager.h"

#include <memory>

namespace Engine {

// Inject the engine context & setup for rendering
RendererManager::RendererManager(EngineContext &engineContext,
                                 EngineState &state)
    : engineContext(engineContext) {

    GPUResourceManager::init(engineContext.getAsset());

    // Set opengl version
    glGetIntegerv(GL_MAJOR_VERSION,
                  &state.renderer.settings.openGlMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION,
                  &state.renderer.settings.openGlMinorVersion);

    // Check for compute shader compatibility
    if (state.renderer.settings.openGlMajorVersion > 4 ||
        (state.renderer.settings.openGlMajorVersion == 4 &&
         state.renderer.settings.openGlMinorVersion >= 6)) {

        // Set compute shader compatiblity
        state.renderer.settings.systemComputeShaderCompatability = true;
    }

    Logger::info("RENDERER", "Renderer Manager initialized.");
}

// Data cleanup
void RendererManager::shutdown() {
    activeRenderer = nullptr;

    for (auto &ittr : rendererRegistry) {
        ittr.second->shutdown();
    }

    rendererRegistry.clear();
}

// Swap the active renderer
void RendererManager::swapActiveRenderer(const RendererHandle handle) {
    const auto &ittr = rendererRegistry.find(handle);
    if (ittr == rendererRegistry.end()) {
        Logger::error(
            "RENDERER",
            "Failed to swap active renderer: render handle not in registry: " +
                std::to_string(handle));
    }

    activeRenderer = ittr->second.get();
}

RendererHandle RendererManager::createNewRenderer() {
    std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>();

    RendererHandle newHandle = UUID();
    rendererRegistry[newHandle] = renderer;

    return newHandle;
}

Renderer *RendererManager::getRenderer(RendererHandle handle) {
    const auto &ittr = rendererRegistry.find(handle);
    if (ittr == rendererRegistry.end()) {
        Logger::error(
            "RENDERER",
            "Failed to locate renderer: render handle not in registry: " +
                std::to_string(handle));
    }

    return ittr->second.get();
}

void RendererManager::destroyRenderer(const RendererHandle handle) {
    const auto &ittr = rendererRegistry.find(handle);
    if (ittr == rendererRegistry.end()) {
        Logger::error(
            "RENDERER",
            "Failed to destroy renderer: render handle not in registry: " +
                std::to_string(handle));
    }

    ittr->second->shutdown();
    rendererRegistry.erase(handle);
}

// Render a frame
void RendererManager::render(EngineState &state) {
    // Render the scene
    START_PROFILE("Render"); // Start timer for render
    activeRenderer->execute(state);
    END_PROFILE("Render"); // End Timer for render
}

// Resize the frame output
void RendererManager::resize(const int newWidth, const int newHeight) {
    activeRenderer->resize(newWidth, newHeight);
}

} // namespace Engine
