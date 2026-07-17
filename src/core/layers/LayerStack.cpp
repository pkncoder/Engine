#include "LayerStack.h"

#include <memory>

namespace Engine {

LayerStack::~LayerStack() {

    // Every onDetach once the layer stack is deleted
    for (auto &layer : layers) {
        layer->onDetach();
    }
}

void LayerStack::dispatchStack() {

    // Loop each layer and call onUpdate
    for (auto &layer : layers) {
        layer->onUpdate(*engineState.get());
    }
}

void LayerStack::dispatchEvent(std::shared_ptr<IEvent> event) const {

    // Loop each layer, run the on event, and if event is consumed stop the loop
    for (const auto &layer : layers) {
        layer->onEvent(event, *engineState.get());

        if (event->isConsumed()) {
            break;
        }
    }
}

} // namespace Engine
