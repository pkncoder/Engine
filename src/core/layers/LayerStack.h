#pragma once

#include "ILayer.h"

#include <memory>
#include <vector>

namespace Engine {

class LayerStack {

  public:
    LayerStack() = default;
    ~LayerStack();

    inline void pushLayer(std::shared_ptr<ILayer> layer) {
        layers.push_back(layer);
    };
    inline void popLayer() { layers.pop_back(); };

    void dispatchStack(EngineState &engineState);

  private:
    std::vector<std::shared_ptr<ILayer>> layers;
};

} // namespace Engine
