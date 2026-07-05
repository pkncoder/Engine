#include "LayerStack.h"

namespace Engine {

LayerStack::~LayerStack() {
    for (auto &layer : layers) {
        layer->onDetach();
    }
}

void LayerStack::dispatchStack(EngineState &engineState) {
    for (auto &layer : layers) {
        layer->onUpdate(engineState);
    }
}

} // namespace Engine
