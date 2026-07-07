#include "LayerStack.h"

#include "../../services/Logger.h"

#include <memory>

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

void LayerStack::dispatchEvent(std::shared_ptr<IEvent> event) {

    Logger::warn("DEBUG - EVENT", "" + event->getType());

    for (auto &layer : layers) {
        layer->onEvent(event);

        if (event->isConsumed()) {
            break;
        }
    }
}

} // namespace Engine
