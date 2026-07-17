#pragma once

#include "../events/IEventDispatcher.h"
#include "ILayer.h"

#include <memory>
#include <vector>

namespace Engine {

class LayerStack : public IEventDispatcher {

  public:
    // Default constructor and then the main constructor
    LayerStack() = default;
    LayerStack(std::shared_ptr<EngineState> engineState)
        : engineState(engineState){};

    // Deconstructor, calls IEvent::onDispatch()
    ~LayerStack();

    // layer vector modifications
    inline void pushLayer(std::shared_ptr<ILayer> layer) {
        layers.push_back(layer);
        layer->onAttach(*engineState);
    };
    inline void popLayer() { layers.pop_back(); };

    // Dispatch events
    void dispatchStack();
    void dispatchEvent(std::shared_ptr<IEvent> event) const override;

  private:
    std::shared_ptr<EngineState> engineState = nullptr;
    std::vector<std::shared_ptr<ILayer>> layers;
};

} // namespace Engine
