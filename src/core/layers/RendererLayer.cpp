#include "RendererLayer.h"
#include "../../services/Logger.h"
#include "../events/KeyEvents.h"
#include "../events/WindowEvents.h"

#include <memory>

namespace Engine {

RendererLayer::RendererLayer(EngineContext &engineContext)
    : engineContext(engineContext) {}

void RendererLayer::onAttach(EngineState &engineState) {
    // TODO: add an init function to rendererManager
    // rendererManager.init();
    engineContext.getRenderer()->resize(engineState.window.width,
                                        engineState.window.height);
}
void RendererLayer::onUpdate(EngineState &engineState) {
    engineContext.getRenderer()->render(engineState);
}

void RendererLayer::onDetach() {
    engineContext.getRenderer()->getActiveRenderer()->shutdown();
}

void RendererLayer::onEvent(std::shared_ptr<IEvent> event, EngineState &state) {
    if (event->getType() == EventType::WINDOW_RESIZE_EVENT) {
        std::shared_ptr<WindowResizeEvent> windowResizeEvent =
            std::static_pointer_cast<WindowResizeEvent>(event);
        engineContext.getRenderer()->resize(windowResizeEvent->windowSize.x,
                                            windowResizeEvent->windowSize.y);
    }

    if (event->getType() == EventType::KEY_PRESS_EVENT) {
        std::shared_ptr<KeyPressEvent> keyPressEvent =
            std::static_pointer_cast<KeyPressEvent>(event);
        if (keyPressEvent->key == Key::R) {

            switch (state.renderer.settings.currentRenderChoice) {
            case RenderChoice::RASTERIZER:

                // Check to see if compute shaders are compatable with this
                // system
                if (state.renderer.settings.systemComputeShaderCompatability) {

                    // If they are, swap the render choice and set it in the
                    // engine state
                    rendererManager.swapActiveRenderer(
                        RenderChoice::PATH_TRACER);
                    state.renderer.settings.currentRenderChoice =
                        RenderChoice::PATH_TRACER;
                } else {

                    // Else, send an error
                    Logger::error("RENDERER",
                                  "Path Tracer not supported on this system.");
                }
                break;
            case RenderChoice::PATH_TRACER:

                // Swap to rasterizer & set the render choice
                rendererManager.swapActiveRenderer(RenderChoice::RASTERIZER);
                state.renderer.settings.currentRenderChoice =
                    RenderChoice::RASTERIZER;
                break;
            }
        }
    }
}

} // namespace Engine
