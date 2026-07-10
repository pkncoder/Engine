#pragma once

#include "../events/IEventDispatcher.h"
#include "ILayer.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace Engine {

class LayerStack : public IEventDispatcher {

  public:
    LayerStack() = default;
    inline LayerStack(std::shared_ptr<EngineState> engineState)
        : engineState(engineState){};
    ~LayerStack();

    inline void pushLayer(std::shared_ptr<ILayer> layer) {
        layers.push_back(layer);
    };
    inline void popLayer() { layers.pop_back(); };

    void dispatchStack();
    void dispatchEvent(std::shared_ptr<IEvent> event);

  private:
    std::vector<std::shared_ptr<ILayer>> layers;
    std::shared_ptr<EngineState> engineState = nullptr;
};

} // namespace Engine
