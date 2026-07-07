#pragma once

#include "../events/IEventDispatcher.h"
#include "ILayer.h"

#include <memory>
#include <vector>

namespace Engine {

class LayerStack : IEventDispatcher {

  public:
    LayerStack() = default;
    ~LayerStack();

    inline void pushLayer(std::shared_ptr<ILayer> layer) {
        layers.push_back(layer);
    };
    inline void popLayer() { layers.pop_back(); };

    void dispatchStack(EngineState &engineState);
    void dispatchEvent(std::shared_ptr<IEvent> event);

  private:
    std::vector<std::shared_ptr<ILayer>> layers;
};

} // namespace Engine
