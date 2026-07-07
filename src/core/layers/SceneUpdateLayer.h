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
    inline void onUpdate(EngineState &engineState) override{};
    inline void onDetach() override{};
    void onEvent(std::shared_ptr<IEvent> event) override;

  private:
    std::shared_ptr<SceneManager> sceneManager;
};

} // namespace Engine
