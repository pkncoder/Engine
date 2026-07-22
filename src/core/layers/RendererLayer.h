#pragma once

#include "ILayer.h"

#include "../EngineContext.h"
namespace Engine {

class RendererLayer : public ILayer {

  public:
    RendererLayer(EngineContext &engineContext);
    ~RendererLayer() = default;

    // Layer state changes
    void onAttach(EngineState &state) override;
    void onUpdate(EngineState &state) override;
    void onDetach() override;
    void onEvent(std::shared_ptr<IEvent> event, EngineState &state) override;

  private:
    // Injected context
    EngineContext &engineContext;
};

} // namespace Engine
