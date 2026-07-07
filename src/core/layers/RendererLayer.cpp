#include "RendererLayer.h"

namespace Engine {

RendererLayer::RendererLayer(RendererManager &rendererManager)
    : rendererManager(rendererManager) {}

void RendererLayer::onAttach(EngineState &engineState) {
    // TODO: add an init function to rendererManager
    // rendererManager.init();
}
void RendererLayer::onUpdate(EngineState &engineState) {
    rendererManager.render(engineState);
}

void RendererLayer::onDetach() {
    rendererManager.getActiveRenderer()->shutdown();
}

void RendererLayer::onEvent(std::shared_ptr<IEvent> event) {}

} // namespace Engine
