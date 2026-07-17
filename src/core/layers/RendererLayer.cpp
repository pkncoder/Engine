#include "RendererLayer.h"
#include "../../services/Logger.h"
#include "../events/KeyEvents.h"
#include "../events/WindowEvents.h"

#include <memory>

namespace Engine {

RendererLayer::RendererLayer(EngineContext &engineContext)
    : engineContext(engineContext) {}

// Constructors
void RendererLayer::onAttach(EngineState &engineState) {
    // TODO: add an init function to rendererManager
    // rendererManager.init();
    engineContext.getRenderer()->resize(engineState.window.width,
                                        engineState.window.height);
}

// Frame update
void RendererLayer::onUpdate(EngineState &engineState) {
    // Render the new frame
    engineContext.getRenderer()->render(engineState);
}

// Cleanup functions
void RendererLayer::onDetach() { engineContext.getRenderer()->shutdown(); }

// Event
void RendererLayer::onEvent(std::shared_ptr<IEvent> event, EngineState &state) {

    // Window resize
    if (event->getType() == EventType::WINDOW_RESIZE_EVENT) {

        // Turn the IEvent into a WindowResizeEvent
        std::shared_ptr<WindowResizeEvent> windowResizeEvent =
            std::static_pointer_cast<WindowResizeEvent>(event);

        // Resize the renderer
        engineContext.getRenderer()->resize(windowResizeEvent->windowSize.x,
                                            windowResizeEvent->windowSize.y);
    }

    // Current renderer choice change
    if (event->getType() == EventType::KEY_PRESS_EVENT) {

        // Get the keypress event
        std::shared_ptr<KeyPressEvent> keyPressEvent =
            std::static_pointer_cast<KeyPressEvent>(event);
        if (keyPressEvent->key ==
            Key::R) { // Make sure it is R for changing the renderer

            // Switch statement to figure out what to change to
            switch (state.renderer.settings.currentRenderChoice) {
            case RenderChoice::RASTERIZER:

                // Check to see if compute shaders are compatable with this
                // system
                if (state.renderer.settings.systemComputeShaderCompatability) {

                    // If they are, swap the render choice and set it in the
                    // engine state
                    engineContext.getRenderer()->swapActiveRenderer(
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
                engineContext.getRenderer()->swapActiveRenderer(
                    RenderChoice::RASTERIZER);
                state.renderer.settings.currentRenderChoice =
                    RenderChoice::RASTERIZER;
                break;
            }
        }
    }
}

} // namespace Engine
