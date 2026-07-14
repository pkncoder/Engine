#pragma once

#include "../EngineContext.h"
#include "ILayer.h"

#include <memory>

namespace Engine {

class SceneUpdateLayer : public ILayer {
  public:
    SceneUpdateLayer(EngineContext &engineContext);
    ~SceneUpdateLayer() = default;

    void onAttach(EngineState &engineState) override;
    void onUpdate(EngineState &engineState) override;
    inline void onDetach() override {};
    void onEvent(std::shared_ptr<IEvent> event, EngineState &state) override;

  private:
    EngineContext &engineContext;
};

} // namespace Engine
