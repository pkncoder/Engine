#pragma once

#include "../EngineContext.h"
#include "ILayer.h"

#include <memory>

namespace Engine {

class SceneUpdateLayer : public ILayer {
  public:
    SceneUpdateLayer(EngineContext &engineContext);
    ~SceneUpdateLayer() = default;

    // Layer state changes
    void onAttach(EngineState &engineState) override;
    void onUpdate(EngineState &engineState) override;
    void onDetach() override;
    void onEvent(std::shared_ptr<IEvent> event, EngineState &state) override;

  private:
    // Injected context
    EngineContext &engineContext;
};

} // namespace Engine
