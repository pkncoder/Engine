#include "RendererLayer.h"
#include "../events/WindowEvents.h"

#include <memory>

namespace Engine {

RendererLayer::RendererLayer(RendererManager &rendererManager)
    : rendererManager(rendererManager) {}

void RendererLayer::onAttach(EngineState &engineState) {
    // TODO: add an init function to rendererManager
    // rendererManager.init();
    rendererManager.resize(engineState.window.width, engineState.window.height);
}
void RendererLayer::onUpdate(EngineState &engineState) {
    rendererManager.render(engineState);
}

void RendererLayer::onDetach() {
    rendererManager.getActiveRenderer()->shutdown();
}

void RendererLayer::onEvent(std::shared_ptr<IEvent> event, EngineState &state) {
    if (event->getType() == EventType::WINDOW_RESIZE_EVENT) {
        std::shared_ptr<WindowResizeEvent> windowResizeEvent =
            std::static_pointer_cast<WindowResizeEvent>(event);
        rendererManager.resize(windowResizeEvent->windowSize.x,
                               windowResizeEvent->windowSize.y);
    }
}

} // namespace Engine
