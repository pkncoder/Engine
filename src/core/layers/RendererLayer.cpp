#include "RendererLayer.h"
#include <memory>

namespace Engine {

RendererLayer::RendererLayer(RendererManager &rendererManager, Window &window)
    : rendererManager(rendererManager), window(window) {}

void RendererLayer::onAttach(EngineState &engineState) {
    // TODO: add an init function to rendererManager
    // rendererManager.init();
}
void RendererLayer::onUpdate(EngineState &engineState) {
    rendererManager.render(window);
}

void RendererLayer::onDetach() {
    rendererManager.getActiveRenderer()->shutdown();
}

} // namespace Engine
