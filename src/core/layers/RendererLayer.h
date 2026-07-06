#pragma once

#include "../../renderer/RendererManager.h"
#include "ILayer.h"

namespace Engine {

class RendererLayer : public ILayer {

  public:
    RendererLayer(RendererManager &rendererManager);
    ~RendererLayer() = default;

    void onAttach(EngineState &state) override;
    void onUpdate(EngineState &state) override;
    void onDetach() override;

  private:
    RendererManager &rendererManager;
};

} // namespace Engine
