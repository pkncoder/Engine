#pragma once

#include "../../renderer/RendererManager.h"
#include "ILayer.h"
#include <memory>

namespace Engine {

class RendererLayer : public ILayer {

  public:
    RendererLayer(RendererManager &rendererManager);
    ~RendererLayer() = default;

    void onAttach(EngineState &state) override;
    void onUpdate(EngineState &state) override;
    void onDetach() override;
    void onEvent(std::shared_ptr<IEvent> event, EngineState &state) override;

  private:
    RendererManager &rendererManager;
};

} // namespace Engine
