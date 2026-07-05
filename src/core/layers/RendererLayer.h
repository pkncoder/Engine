#pragma once

#include "../../renderer/RendererManager.h"
#include "../Window.h"
#include "ILayer.h"

#include <memory>

namespace Engine {

class RendererLayer : public ILayer {

  public:
    RendererLayer(RendererManager &rendererManager, Window &window);
    ~RendererLayer() = default;

    void onAttach(EngineState &state) override;
    void onUpdate(EngineState &state) override;
    void onDetach() override;

  private:
    RendererManager &rendererManager;
    Window &window;
};

} // namespace Engine
