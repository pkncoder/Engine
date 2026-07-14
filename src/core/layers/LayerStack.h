#pragma once

#include "../events/IEventDispatcher.h"
#include "ILayer.h"

#include <memory>
#include <vector>

namespace Engine {

class LayerStack : public IEventDispatcher {

  public:
    LayerStack() = default;
    LayerStack(std::shared_ptr<EngineState> engineState)
        : engineState(engineState) {};
    ~LayerStack();

    inline void pushLayer(std::shared_ptr<ILayer> layer) {
        layers.push_back(layer);
        layer->onAttach(*engineState);
    };
    inline void popLayer() { layers.pop_back(); };

    void dispatchStack();
    void dispatchEvent(std::shared_ptr<IEvent> event);

  private:
    std::vector<std::shared_ptr<ILayer>> layers;
    std::shared_ptr<EngineState> engineState = nullptr;
};

} // namespace Engine
