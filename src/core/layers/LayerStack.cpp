#include "LayerStack.h"

#include "../../services/Logger.h"

#include <memory>

namespace Engine {

LayerStack::~LayerStack() {
    for (auto &layer : layers) {
        layer->onDetach();
    }
}

void LayerStack::dispatchStack() {
    for (auto &layer : layers) {
        layer->onUpdate(*engineState.get());
    }
}

void LayerStack::dispatchEvent(std::shared_ptr<IEvent> event) {

    Logger::warn("DEBUG - EVENT", "" + event->getType());

    for (auto &layer : layers) {
        layer->onEvent(event, *engineState.get());

        if (event->isConsumed()) {
            break;
        }
    }
}

} // namespace Engine
