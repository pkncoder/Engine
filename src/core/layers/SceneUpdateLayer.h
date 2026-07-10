#pragma once

#include "../../scene/SceneManager.h"
#include "ILayer.h"

#include <memory>

namespace Engine {

class SceneUpdateLayer : public ILayer {
  public:
    SceneUpdateLayer(std::shared_ptr<SceneManager> sceneManager);
    ~SceneUpdateLayer() = default;

    void onAttach(EngineState &engineState) override;
    void onUpdate(EngineState &engineState) override;
    inline void onDetach() override{};
    void onEvent(std::shared_ptr<IEvent> event, EngineState &state) override;

  private:
    std::shared_ptr<SceneManager> sceneManager;
};

} // namespace Engine
